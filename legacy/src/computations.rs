use crate::data_structures;

use rayon::prelude::*;
use rgsl::ComplexF64;
use std::io::Write;

pub fn compute_linear_sampling(scale: i64, scale_begin: i64, scale_end: i64, value_begin: f64, value_end: f64) -> f64 {
    if scale_begin == scale_end {
        (value_begin + value_end) / 2.0
    } else {
        (scale as f64 / (scale_end - scale_begin + 1) as f64) * (value_end - value_begin) + value_begin
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


pub fn compute_approx_complex_derivative(left: &ComplexF64, right: &ComplexF64, diff: f64) -> ComplexF64 {
    return ComplexF64::rect(
        right.dat[0] - left.dat[0] / (2.0 * diff),
        right.dat[1] - left.dat[1] / (2.0 * diff),
    );
}


pub async fn compute_and_export(config: &data_structures::ConfigPacket) -> Result<(), ComputationError> {
    let output_id: String = rand::random::<u16>().to_string();

    let mut cell_count: (u64, u64, u64) = (
        ((config.simulation_geometry.end.0 - config.simulation_geometry.begin.0).abs() / config.simulation_geometry.cell_size) as u64 + 1,
        ((config.simulation_geometry.end.1 - config.simulation_geometry.begin.1).abs() / config.simulation_geometry.cell_size) as u64 + 1,
        ((config.simulation_geometry.end.2 - config.simulation_geometry.begin.2).abs() / config.simulation_geometry.cell_size) as u64 + 1,
    );

    // Raw pressure calculation is done slightly outside the range to allow for derivative calculation.
    let mut pressure_complex_field: Vec<Vec<Vec<ComplexF64>>> = vec![vec![vec![ComplexF64::rect(0.0, 0.0); (cell_count.2 + 2) as usize]; (cell_count.1 + 2) as usize]; (cell_count.0 + 2) as usize];

    // All array index are offset to account for extra computation done.
    for axis_0 in 0..cell_count.0 + 2 {


        print!("{} ", compute_linear_sampling(axis_0 as i64, 1, cell_count.0 as i64, config.simulation_geometry.begin.0, config.simulation_geometry.end.0));

        for axis_1 in 0..cell_count.1 + 2 {
            for axis_2 in 0..cell_count.2 + 2 {
                let position: (f64, f64, f64) = (
                    compute_linear_sampling(axis_0 as i64, 1, cell_count.0 as i64, config.simulation_geometry.begin.0, config.simulation_geometry.end.0),
                    compute_linear_sampling(axis_1 as i64, 1, cell_count.1 as i64, config.simulation_geometry.begin.1, config.simulation_geometry.end.1),
                    compute_linear_sampling(axis_2 as i64, 1, cell_count.2 as i64, config.simulation_geometry.begin.2, config.simulation_geometry.end.2),
                );

                pressure_complex_field[axis_0 as usize][axis_1 as usize][axis_2 as usize] = config.transducers.iter()
                    .fold(ComplexF64::rect(0.0, 0.0), |acc, val: &data_structures::Transducer| {
                        acc.add(&compute_point_pressure(&position, &val))
                    });
            }
        }
    }

    let pressure_abs_squared: Vec<Vec<Vec<f64>>> = pressure_complex_field.par_iter().map(
        |page: &Vec<Vec<ComplexF64>>| {
            page
                .iter()
                .map(|row: &Vec<ComplexF64>| {
                    row
                        .iter()
                        .map(|value| {
                            value.abs2()
                        })
                        .collect()
                })
                .collect()
        })
        .collect();

    let empty_field: Vec<Vec<Vec<f64>>> = vec![vec![vec![0.0; (cell_count.2 + 2) as usize]; (cell_count.1 + 2) as usize]; (cell_count.0 + 2) as usize];

    let mut derivate_x_abs_squared: Vec<Vec<Vec<f64>>> = empty_field.clone();
    let mut derivate_y_abs_squared: Vec<Vec<Vec<f64>>> = empty_field.clone();
    let mut derivate_z_abs_squared: Vec<Vec<Vec<f64>>> = empty_field.clone();

    for axis_0 in 1..cell_count.0 + 1 {
        for axis_1 in 1..cell_count.1 + 1 {
            for axis_2 in 1..cell_count.2 + 1 {
                derivate_x_abs_squared[axis_0 as usize][axis_1 as usize][axis_2 as usize] = compute_approx_complex_derivative(
                    &pressure_complex_field[axis_0 as usize - 1][axis_1 as usize][axis_2 as usize], &pressure_complex_field[axis_0 as usize + 1][axis_1 as usize][axis_2 as usize], config.simulation_geometry.cell_size,
                ).abs2();
                derivate_y_abs_squared[axis_0 as usize][axis_1 as usize][axis_2 as usize] = compute_approx_complex_derivative(
                    &pressure_complex_field[axis_0 as usize][axis_1 as usize - 1][axis_2 as usize], &pressure_complex_field[axis_0 as usize][axis_1 as usize + 1][axis_2 as usize], config.simulation_geometry.cell_size,
                ).abs2();
                derivate_z_abs_squared[axis_0 as usize][axis_1 as usize][axis_2 as usize] = compute_approx_complex_derivative(
                    &pressure_complex_field[axis_0 as usize][axis_1 as usize][axis_2 as usize - 1], &pressure_complex_field[axis_0 as usize][axis_1 as usize][axis_2 as usize + 1], config.simulation_geometry.cell_size,
                ).abs2();
            }
        }
    }


    let mut potential_field: Vec<Vec<Vec<f64>>> = empty_field.clone();

    for axis_0 in 1..cell_count.0 + 1 {
        for axis_1 in 1..cell_count.1 + 1 {
            for axis_2 in 1..cell_count.2 + 1 {
                potential_field[axis_0 as usize][axis_1 as usize][axis_2 as usize] =
                    2.0 * config.simulation_geometry.potential_compute_const_1 * pressure_abs_squared[axis_0 as usize][axis_1 as usize][axis_2 as usize] -
                        2.0 * config.simulation_geometry.potential_compute_const_2 * (
                            derivate_x_abs_squared[axis_0 as usize][axis_1 as usize][axis_2 as usize] +
                                derivate_y_abs_squared[axis_0 as usize][axis_1 as usize][axis_2 as usize] +
                                derivate_z_abs_squared[axis_0 as usize][axis_1 as usize][axis_2 as usize]
                        );
            }
        }
    }


    let plane_count: (u64, u64, u64) = match config.simulation_geometry.plane {
        'X' => (cell_count.0, cell_count.1, cell_count.2),
        'Y' => (cell_count.1, cell_count.0, cell_count.2),
        _ => (cell_count.2, cell_count.1, cell_count.0),
    };


    for axis_0 in 1..plane_count.0 + 1 {
        let empty_result = vec![vec![0.0; plane_count.2 as usize + 2]; plane_count.1 as usize + 2];

        let mut result_pressure_abs = empty_result.clone();
        let mut result_pressure_real = empty_result.clone();
        let mut result_pressure_imag = empty_result.clone();
        let mut result_potential = empty_result.clone();

        for axis_1 in 1..plane_count.1 + 1 {
            for axis_2 in 1..plane_count.2 + 1 {

                let array_ref_idx: (usize, usize, usize) = match config.simulation_geometry.plane {
                    'X' => (axis_0 as usize, axis_1 as usize,  axis_2 as usize),
                    'Y' => (axis_1 as usize, axis_0 as usize,  axis_2 as usize),
                    _ =>   (axis_2 as usize, axis_1 as usize,  axis_0 as usize),
                };

                result_pressure_abs[(axis_1 - 1) as usize][(axis_2 - 1) as usize] = pressure_complex_field[array_ref_idx.0 as usize][array_ref_idx.1 as usize][array_ref_idx.2 as usize].abs();
                result_pressure_real[(axis_1 - 1) as usize][(axis_2 - 1) as usize] = pressure_complex_field[array_ref_idx.0 as usize][array_ref_idx.1 as usize][array_ref_idx.2 as usize].dat[0];
                result_pressure_imag[(axis_1 - 1) as usize][(axis_2 - 1) as usize] = pressure_complex_field[array_ref_idx.0 as usize][array_ref_idx.1 as usize][array_ref_idx.2 as usize].dat[1];
                result_potential[(axis_1 - 1) as usize][(axis_2 - 1) as usize] = potential_field[array_ref_idx.0 as usize][array_ref_idx.1 as usize][array_ref_idx.2 as usize];
            }
        }

        export_output(
            format!("outputabs{}_{}{}.csv", output_id, config.simulation_geometry.plane, axis_0).as_str(),
            format!("Pressure Complex Amplitude Value Output,Plane {}, Row-Column Axis: {}", config.simulation_geometry.plane,
                    match config.simulation_geometry.plane {
                        'X' => "Y-Z",
                        'Y' => "X-Z",
                        'Z' => "Y-X",
                        _ => "Axis Mismatched!"
                    }).as_str(),
            &result_pressure_abs);

        export_output(
            format!("outputreal{}_{}{}.csv", output_id, config.simulation_geometry.plane, axis_0).as_str(),
            format!("Pressure Complex Real Value Output,Plane {}, Row-Column Axis: {}", config.simulation_geometry.plane,
                    match config.simulation_geometry.plane {
                        'X' => "Y-Z",
                        'Y' => "X-Z",
                        'Z' => "Y-X",
                        _ => "Axis Mismatched!"
                    }).as_str(),
            &result_pressure_real);

        export_output(
            format!("outputimag{}_{}{}.csv", output_id, config.simulation_geometry.plane, axis_0).as_str(),
            format!("Pressure Complex Imaginary Value Output,Plane {}, Row-Column Axis: {}", config.simulation_geometry.plane,
                    match config.simulation_geometry.plane {
                        'X' => "Y-Z",
                        'Y' => "X-Z",
                        'Z' => "Y-X",
                        _ => "Axis Mismatched!"
                    }).as_str(),
            &result_pressure_imag);

        export_output(
            format!("outputpotential{}_{}{}.csv", output_id, config.simulation_geometry.plane, axis_0).as_str(),
            format!("Potential Field Value Output,Plane {}, Row-Column Axis: {}", config.simulation_geometry.plane,
                    match config.simulation_geometry.plane {
                        'X' => "Y-Z",
                        'Y' => "X-Z",
                        'Z' => "Y-X",
                        _ => "Axis Mismatched!"
                    }).as_str(),
            &result_potential);
    }


    Ok(())
}

fn export_output(filename: &str, csv_header: &str, csv_content: &Vec<Vec<f64>>) {
    const ERROR_TEXT: &str = "Failed to write output files.";
    let mut file = std::fs::File::create(filename).expect(ERROR_TEXT);
    file.write(csv_header.as_bytes()).expect(ERROR_TEXT);
    file.write("\n".as_bytes()).expect(ERROR_TEXT);
    for line in csv_content.iter() {
        for elem in line.iter() {
            file.write(elem.to_string().as_bytes()).expect(ERROR_TEXT);
            file.write(",".as_bytes()).expect(ERROR_TEXT);
        }
        file.write("\n".as_bytes()).expect(ERROR_TEXT);
    }
    file.flush().expect(ERROR_TEXT);
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