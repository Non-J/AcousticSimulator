use crate::data_structures;

use rayon::prelude::*;
use rgsl::ComplexF64;
use std::io::Write;

pub fn compute_linear_sampling(scale: u64, scale_begin: u64, scale_end: u64, value_begin: f64, value_end: f64) -> f64 {
    if scale_begin == scale_end {
        (value_begin + value_end) / 2.0
    } else {
        (scale as f64 / (scale_end - scale_begin) as f64) * (value_end - value_begin) + value_begin
    }
}

pub fn compute_distance_between(first: &(f64, f64, f64), second: &(f64, f64, f64)) -> f64 {
    ((first.0 - second.0).powi(2) + (first.1 - second.1).powi(2) + (first.2 - second.2).powi(2)).sqrt()
}

pub fn compute_angle_between(origin: &(f64, f64, f64), first: &(f64, f64, f64), second: &(f64, f64, f64)) -> f64 {
    let vec_first = (first.0 - origin.0, first.1 - origin.1, first.2 - origin.2);
    let vec_second = (second.0 - origin.0, second.1 - origin.1, second.2 - origin.2);

    let dot = (vec_first.0 * vec_second.0) + (vec_first.1 * vec_second.1) + (vec_first.2 * vec_second.2);
    let denom_first = (vec_first.0 * vec_first.0) + (vec_first.1 * vec_first.1) + (vec_first.2 * vec_first.2);
    let denom_second = (vec_second.0 * vec_second.0) + (vec_second.1 * vec_second.1) + (vec_second.2 * vec_second.2);

    (dot / (denom_first.sqrt() * denom_second.sqrt())).acos()
}


pub fn compute_point_pressure(point: &(f64, f64, f64), transducer: &data_structures::Transducer) -> ComplexF64 {
    let angle = compute_angle_between(&transducer.position, &transducer.target, &point);
    let dist = compute_distance_between(&transducer.position, &point);
    let wave_number = 2.0 * std::f64::consts::PI / transducer.wavelength;

    let intermediate = wave_number * transducer.radius * angle.sin();
    let directivity = if intermediate == 0.0 { 1.0 } else { rgsl::bessel::j1(intermediate) / intermediate * 2.0 };

    ComplexF64::rect(0.0, wave_number * dist + transducer.phase_shift).exp().mul_real(transducer.output_power * transducer.loss_factor * directivity / dist)
}


pub async fn compute_and_export(config: &data_structures::ConfigPacket) -> Result<(), ComputationError> {

    // Remap values into tuple so that the actual code can be generic on simulation plane
    let axis_count = match config.simulation_geometry.plane {
        'X' => { (config.simulation_geometry.division.0, config.simulation_geometry.division.1, config.simulation_geometry.division.2) }
        'Y' => { (config.simulation_geometry.division.1, config.simulation_geometry.division.0, config.simulation_geometry.division.2) }
        'Z' => { (config.simulation_geometry.division.2, config.simulation_geometry.division.1, config.simulation_geometry.division.0) }
        _ => {
            return Err(ComputationError::new("Simulation Geometry Plane is invalid"));
        }
    };

    let axis_begin = match config.simulation_geometry.plane {
        'X' => { (config.simulation_geometry.begin.0, config.simulation_geometry.begin.1, config.simulation_geometry.begin.2) }
        'Y' => { (config.simulation_geometry.begin.1, config.simulation_geometry.begin.0, config.simulation_geometry.begin.2) }
        'Z' => { (config.simulation_geometry.begin.2, config.simulation_geometry.begin.1, config.simulation_geometry.begin.0) }
        _ => {
            return Err(ComputationError::new("Simulation Geometry Plane is invalid"));
        }
    };

    let axis_end = match config.simulation_geometry.plane {
        'X' => { (config.simulation_geometry.end.0, config.simulation_geometry.end.1, config.simulation_geometry.end.2) }
        'Y' => { (config.simulation_geometry.end.1, config.simulation_geometry.end.0, config.simulation_geometry.end.2) }
        'Z' => { (config.simulation_geometry.end.2, config.simulation_geometry.end.1, config.simulation_geometry.end.0) }
        _ => {
            return Err(ComputationError::new("Simulation Geometry Plane is invalid"));
        }
    };

    let output_id: String = rand::random::<u16>().to_string();

    // Outer Most Axis is used as the parallel thread
    (0..axis_count.0).into_par_iter()
        .for_each(
            |axis_0: i64| {
                // Computation of 2D Vec result
                let result: Vec<Vec<f64>> =
                    (0..axis_count.1).into_iter()
                        .map(
                            |axis_1: i64| {
                                (0..axis_count.2)
                                    .into_iter()
                                    .map(
                                        |axis_2: i64| {
                                            let point = (
                                                compute_linear_sampling(axis_0 as u64, 0, (axis_count.0 - 1) as u64, axis_begin.0, axis_end.0),
                                                compute_linear_sampling(axis_1 as u64, 0, (axis_count.1 - 1) as u64, axis_begin.1, axis_end.1),
                                                compute_linear_sampling(axis_2 as u64, 0, (axis_count.2 - 1) as u64, axis_begin.2, axis_end.2),
                                            );
                                            config.transducers.iter()
                                                .fold(ComplexF64::rect(0.0, 0.0), |acc, val: &data_structures::Transducer| {
                                                    acc.add(&compute_point_pressure(&point, &val))
                                                }).abs()
                                        }
                                    )
                                    .collect()
                            }
                        )
                        .collect();

                // Output
                export_output(
                    format!("output{}_{}{}.csv", output_id, config.simulation_geometry.plane, axis_0).as_str(),
                    format!("Plane {} = {}, Row-Column Axis: {}", config.simulation_geometry.plane, compute_linear_sampling(axis_0 as u64, 0, (axis_count.0 - 1) as u64, axis_begin.0, axis_end.0),
                            match config.simulation_geometry.plane {
                                'X' => "Y-Z",
                                'Y' => "X-Z",
                                'Z' => "Y-X",
                                _ => "Axis Mismatched!"
                            }).as_str(),
                    &result);
            }
        );

    Ok(())
}

fn export_output(filename: &str, csv_header: &str, csv_content: &Vec<Vec<f64>>) {
    let mut file = std::fs::File::create(filename).expect("Unable to write output files.");
    file.write(csv_header.as_bytes()).expect("Failed to write output files.");
    file.write("\n".as_bytes()).expect("Failed to write output files.");
    for line in csv_content.iter() {
        for elem in line.iter() {
            file.write(elem.to_string().as_bytes()).expect("Failed to write output files.");
            file.write(",".as_bytes()).expect("Failed to write output files.");
        }
        file.write("\n".as_bytes()).expect("Failed to write output files.");
    }
    file.flush().expect("Failed to write output files.");
}

#[derive(Debug)]
pub struct ComputationError {
    details: String
}

impl ComputationError {
    fn new(message: &str) -> ComputationError {
        ComputationError { details: message.to_string() }
    }
}

impl std::fmt::Display for ComputationError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "{}", self.details)
    }
}

impl std::error::Error for ComputationError {
    fn description(&self) -> &str {
        &self.details
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_compute_distance() {
        ulps_eq!(compute_distance_between(&(0.0, 0.0, 0.0), &(0.0, 0.0, 0.0)), 0.0);
        ulps_eq!(compute_distance_between(&(0.0, 17.0, 0.0), &(0.0, 0.0, 0.0)), 17.0);
        ulps_eq!(compute_distance_between(&(-3.0, -3.0, -3.0), &(4.0, 4.0, 4.0)), 7.0 * 3.0_f64.sqrt());
    }

    #[test]
    fn test_compute_angle() {
        ulps_eq!(compute_angle_between(&(0.0, 0.0, 0.0), &(1.0, 0.0, 0.0), &(0.0, 0.0, 1.0)), 0.5 * std::f64::consts::PI);
        ulps_eq!(compute_angle_between(&(10.0, 6.0, -4.0), &(10.0, 6.0, -6.0), &(10.0, 10000000.0, -4.0)), 0.5 * std::f64::consts::PI);
        ulps_eq!(compute_angle_between(&(7.0, -77.0, 777.0), &(9.0, -77.0, 777.0), &(9.0, -75.0, 777.0)), std::f64::consts::PI / 4.0);
        ulps_eq!(compute_angle_between(&(7.0, -77.0, 777.0), &(9.0, -77.0, 777.0), &(9.0, -73.0, 777.0)), std::f64::consts::PI / 3.0);
    }

    #[test]
    fn test_compute_linear_sampling() {
        ulps_eq!(compute_linear_sampling(0, 0, 0, 3.0, 5.0), 4.0);
        ulps_eq!(compute_linear_sampling(0, 0, 0, 5.0, 3.0), 4.0);
        ulps_eq!(compute_linear_sampling(0, 0, 0, -3.0, -5.0), -4.0);
        ulps_eq!(compute_linear_sampling(0, 0, 0, -5.0, -3.0), -4.0);

        ulps_eq!(compute_linear_sampling(0, 1, 5, 100.0, 500.0), 0.0);
        ulps_eq!(compute_linear_sampling(1, 1, 5, 100.0, 500.0), 100.0);
        ulps_eq!(compute_linear_sampling(2, 1, 5, 100.0, 500.0), 200.0);
        ulps_eq!(compute_linear_sampling(3, 1, 5, 100.0, 500.0), 300.0);
        ulps_eq!(compute_linear_sampling(4, 1, 5, 100.0, 500.0), 400.0);
        ulps_eq!(compute_linear_sampling(5, 1, 5, 100.0, 500.0), 500.0);
        ulps_eq!(compute_linear_sampling(6, 1, 5, 100.0, 500.0), 600.0);

        ulps_eq!(compute_linear_sampling(0, 1, 5, 500.0, 100.0), 600.0);
        ulps_eq!(compute_linear_sampling(1, 1, 5, 500.0, 100.0), 500.0);
        ulps_eq!(compute_linear_sampling(2, 1, 5, 500.0, 100.0), 400.0);
        ulps_eq!(compute_linear_sampling(3, 1, 5, 500.0, 100.0), 300.0);
        ulps_eq!(compute_linear_sampling(4, 1, 5, 500.0, 100.0), 200.0);
        ulps_eq!(compute_linear_sampling(5, 1, 5, 500.0, 100.0), 100.0);
        ulps_eq!(compute_linear_sampling(6, 1, 5, 500.0, 100.0), 0.0);
    }
}