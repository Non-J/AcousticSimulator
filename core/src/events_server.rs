use actix::*;
use actix_web_actors::ws;
use std::collections::HashMap;
use actix_web::{HttpRequest, web, HttpResponse};

/// BEGIN: Actor System Messages

// Message WsServer sent to WsSession
#[derive(Clone, Message)]
#[rtype(result = "()")]
pub struct SessionMessage(pub String);

// When new WsSession connects to WsServer
#[derive(Message)]
#[rtype(result = "()")]
pub struct Connect {
    pub receiver: Recipient<SessionMessage>,
    pub channel: String,
}

// Send message to specific channel
#[derive(Message)]
#[rtype(result = "()")]
pub struct PushMessage {
    pub channel: String,
    pub message: String,
}

/// END: Actor System Messages


/// BEGIN: WsServer
#[derive(Debug)]
pub struct WsServer {
    channels: HashMap<String, Vec<Recipient<SessionMessage>>>,
}

impl WsServer {
    pub fn new() -> Self {
        WsServer { channels: HashMap::new() }
    }
}

impl WsServer {
    fn send_message(&mut self, channel_name: &str, message: &str) {
        let channel = (&mut self.channels).entry(channel_name.to_string()).or_insert(Vec::new());
        channel.retain(|client| {
            client.do_send(SessionMessage { 0: message.to_string() }).is_ok()
        });
    }
}

impl Actor for WsServer {
    type Context = Context<Self>;
}

impl Handler<Connect> for WsServer {
    type Result = ();

    fn handle(&mut self, msg: Connect, _: &mut Context<Self>) {
        self.channels.entry(msg.channel).or_insert(Vec::new()).push(msg.receiver);
    }
}

impl Handler<PushMessage> for WsServer {
    type Result = ();

    fn handle(&mut self, msg: PushMessage, _: &mut Context<Self>) {
        self.send_message(&msg.channel, &msg.message);
    }
}

/// END: WsServer


/// BEGIN: WsSession
#[derive(Debug)]
struct WsSession {
    channel: String,
    server_address: actix::Addr<WsServer>,
}

impl Actor for WsSession {
    type Context = ws::WebsocketContext<Self>;

    fn started(&mut self, ctx: &mut Self::Context) {
        self.server_address
            .send(Connect {
                channel: self.channel.clone(),
                receiver: ctx.address().recipient(),
            })
            .into_actor(self)
            .then(|_res, _act, _ctx| {
                fut::ready(())
            })
            .wait(ctx);
    }

    fn stopping(&mut self, _: &mut Self::Context) -> Running {
        Running::Stop
    }
}

impl Handler<SessionMessage> for WsSession {
    type Result = ();

    fn handle(&mut self, msg: SessionMessage, ctx: &mut Self::Context) {
        ctx.text(msg.0);
    }
}

impl StreamHandler<Result<ws::Message, ws::ProtocolError>> for WsSession {
    fn handle(
        &mut self,
        message: Result<ws::Message, ws::ProtocolError>,
        ctx: &mut Self::Context,
    ) {
        let message = match message {
            Err(_) => {
                ctx.stop();
                return;
            }
            Ok(message) => message,
        };

        match message {
            ws::Message::Ping(message) => {
                ctx.pong(&message);
            }
            ws::Message::Pong(_) => {}
            ws::Message::Text(text) => {
                self.server_address.do_send(PushMessage {
                    channel: self.channel.clone(),
                    message: text,
                })
            }
            ws::Message::Binary(_) => ctx.text("/BinaryNotAccepted"),
            ws::Message::Close(_) => {
                ctx.stop();
            }
            ws::Message::Continuation(_) => {
                ctx.stop();
            }
            ws::Message::Nop => (),
        }
    }
}

/// END: WsSession

pub async fn ws_events(
    req: HttpRequest,
    stream: web::Payload,
    server_address: web::Data<Addr<WsServer>>,
    channel_name: web::Path<String>,
) -> Result<HttpResponse, actix_web::Error> {
    ws::start(
        WsSession {
            channel: channel_name.to_owned(),
            server_address: server_address.get_ref().clone(),
        },
        &req,
        stream,
    )
}