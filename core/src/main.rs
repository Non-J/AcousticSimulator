#[macro_use]
extern crate approx;
#[macro_use]
extern crate actix_web;

mod data_structures;
mod computations;
mod events_server;

use actix_web::{
    web, HttpResponse,
};
use actix::Actor;

#[get("/")]
async fn get_home() -> impl actix_web::Responder {
    actix_files::NamedFile::open("web/index.html")
}

struct SharedAPIData {
    config: std::sync::Mutex<data_structures::ConfigPacket>,
}

#[get("/data")]
async fn get_data(data: web::Data<SharedAPIData>) -> impl actix_web::Responder {
    HttpResponse::Ok()
        .content_type("application/json")
        .body(
            serde_json::to_string(&*data.config.lock().unwrap()).unwrap()
        )
}

#[post("/data")]
async fn post_data((data, body): (web::Data<SharedAPIData>, web::Json<data_structures::ConfigPacket>)) -> impl actix_web::Responder {
    if body.validate() {
        let mut config_access = data.config.lock().unwrap();
        *config_access = body.into_inner();
        config_access.save("data.json").await.unwrap();
        HttpResponse::Accepted()
    } else {
        HttpResponse::BadRequest()
    }
}

#[get("/run_computation")]
async fn get_run_computation(data: web::Data<SharedAPIData>) -> impl actix_web::Responder {
    computations::compute_and_export(&*data.config.lock().unwrap()).await.unwrap();
    HttpResponse::Ok()
}

#[actix_rt::main]
async fn main() -> std::io::Result<()> {
    env_logger::init();

    let shared_data_packet = web::Data::new(SharedAPIData {
        config: std::sync::Mutex::new(data_structures::ConfigPacket::load("data.json").await),
    });

    let ws_event_sys = events_server::WsServer::new().start();

    let server = actix_web::HttpServer::new(move ||
        actix_web::App::new()
            .wrap(actix_web::middleware::NormalizePath)
            .wrap(actix_web::middleware::Logger::default())
            .service(get_home)
            .service(
                web::scope("/api")
                    .app_data(shared_data_packet.clone())
                    .data(ws_event_sys.clone())
                    .service(get_data)
                    .service(post_data)
                    .service(get_run_computation)
                    .route("/wsevent/{channel_name}", web::get().to(events_server::ws_events))
            )
            .default_service(
                web::resource("")
                    .route(web::get().to(HttpResponse::NotFound))
                    .route(
                        web::route()
                            .guard(actix_web::guard::Not(actix_web::guard::Get()))
                            .to(HttpResponse::MethodNotAllowed),
                    ),
            )
    )
        .bind("127.0.0.1:1200")
        .unwrap()
        .run();

    println!("Starting...");
    if open::that("http://localhost:1200").is_ok() {
        println!("To reopen the interface, use this link: http://localhost:1200");
    } else {
        println!("To open the interface, use this link: http://localhost:1200");
    }

    server.await
}