use serde::{Deserialize, Serialize};

#[derive(Debug, Serialize, Deserialize)]
pub struct Transducer {
    pub id: String,

    pub position: (f64, f64, f64),
    pub target: (f64, f64, f64),

    pub radius: f64,
    pub phase_shift: f64,
    pub loss_factor: f64,
    pub output_power: f64,
    pub wavelength: f64,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct SimulationGeometry {
    pub plane: char,
    pub begin: (f64, f64, f64),
    pub end: (f64, f64, f64),
    pub division: (i64, i64, i64),
}

#[derive(Debug, Serialize, Deserialize)]
pub struct ConfigPacket {
    pub transducers: Vec<Transducer>,
    pub simulation_geometry: SimulationGeometry,
}

impl ConfigPacket {
    pub fn new() -> Self {
        ConfigPacket {
            transducers: Vec::new(),
            simulation_geometry: SimulationGeometry {
                plane: 'X',
                begin: (0.0, 0.0, 0.0),
                end: (0.0, 0.0, 0.0),
                division: (1, 1, 1),
            },
        }
    }

    pub async fn load(filename: &str) -> ConfigPacket {
        let file = tokio::fs::read(filename).await;
        let mut data: ConfigPacket = ConfigPacket::new();

        if file.is_ok() {
            data = serde_json::from_slice(file.unwrap().as_slice()).unwrap_or(data);
        }

        data
    }

    pub async fn save(&self, filename: &str) -> Result<(), std::io::Error> {
        tokio::fs::write(filename, serde_json::to_string(&self).unwrap()).await
    }

    pub fn validate(&self) -> bool {
        match self.simulation_geometry.plane {
            'X' | 'Y' | 'Z' => (),
            _ => return false,
        }

        if self.simulation_geometry.division.0 < 1 ||
            self.simulation_geometry.division.1 < 1 ||
            self.simulation_geometry.division.2 < 1 {
            return false;
        }

        for transducer in &self.transducers {
            if transducer.position.0 == transducer.target.0 &&
                transducer.position.1 == transducer.target.1 &&
                transducer.position.2 == transducer.target.2 {
                return false;
            }

            if transducer.radius <= 0.0 || transducer.wavelength <= 0.0 ||
                transducer.loss_factor > 1.0 || transducer.loss_factor < 0.0 ||
                transducer.output_power > 1.0 || transducer.output_power < 0.0 {
                return false;
            }
        }

        true
    }
}
