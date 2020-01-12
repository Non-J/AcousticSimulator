export class Result {
	constructor(ok, value) {
		this.ok = Boolean(ok);
		this.value = value;
	}

	get isOk() {
		return this.ok;
	}

	get isErr() {
		return !this.ok;
	}

	unwrap() {
		if (this.ok) {
			return this.value;
		} else {
			return null;
		}
	}

	unwrap_or(value) {
		if (this.ok) {
			return this.value;
		} else {
			return value;
		}
	}

	expect(err) {
		if (this.ok) {
			return this.value;
		} else {
			throw err;
		}
	}

	map(fn) {
		if (this.ok) {
			return fn(this.value);
		} else {
			return this;
		}
	}
};

export function parseInt(value) {
	let parsed = Number(value);
	if (Number.isNaN(parsed)) {
		return new Result(false, "NotNumber");
	} else if (!Number.isSafeInteger(parsed)) {
		return new Result(false, "NotInteger");
	} else {
		return new Result(true, parsed);
	}
}

export function parseNumber(value) {
	let parsed = Number(value);
	if (Number.isNaN(parsed)) {
		return new Result(false, "NotNumber");
	} else {
		return new Result(true, parsed);
	}
}

class Transducer {
	constructor() {
		this.id = "";
		this.position = [0, 0, 0];
		this.target = [0, 1, 0];
		this.radius = 0.5;
		this.phase_shift = 0;
		this.loss_factor = 1;
		this.output_power = 1;
		this.wavelength = 0.8;
	}

	get tableRowData() {
		return {
			id: this.id,
			position_x: this.position[0],
			position_y: this.position[1],
			position_z: this.position[2],
			target_x: this.target[0],
			target_y: this.target[1],
			target_z: this.target[2],
			radius: this.radius,
			phase_shift: this.phase_shift,
			loss_factor: this.loss_factor,
			output_power: this.output_power,
			wavelength: this.wavelength,
			row_id: Math.random()
		}
	}

	set tableRowData(value) {
		this.id = value['id'] || this.id;
		this.position[0] = value['position_x'] || this.position[0];
		this.position[1] = value['position_y'] || this.position[1];
		this.position[2] = value['position_z'] || this.position[2];
		this.target[0] = value['target_x'] || this.target[0];
		this.target[1] = value['target_y'] || this.target[1];
		this.target[2] = value['target_z'] || this.target[2];
		this.radius = value['radius'] || this.radius;
		this.phase_shift = value['phase_shift'] || this.phase_shift;
		this.loss_factor = value['loss_factor'] || this.loss_factor;
		this.output_power = value['output_power'] || this.output_power;
		this.wavelength = value['wavelength'] || this.wavelength;
	}

	validate() {
		// TODO: Implement
		return true;
	}
}

class SimulationGeometry {
	constructor() {
		this.plane = "X";
		this.begin = [10, 0, -10];
		this.end = [10, 0, 10];
		this.division = [250, 0, 250];
	}

	get flatData() {
		return {
			plane: this.plane,
			begin_x: this.begin[0],
			begin_y: this.begin[1],
			begin_z: this.begin[2],
			end_x: this.end[0],
			end_y: this.end[1],
			end_z: this.end[2],
			division_x: this.division[0],
			division_y: this.division[1],
			division_z: this.division[2],
		}
	}

	set flatData(value) {
		this.plane = value['plane'] || this.plane;
		this.begin[0] = value['begin_x'] || this.begin[0];
		this.begin[1] = value['begin_y'] || this.begin[1];
		this.begin[2] = value['begin_z'] || this.begin[2];
		this.end[0] = value['end_x'] || this.end[0];
		this.end[1] = value['end_y'] || this.end[1];
		this.end[2] = value['end_z'] || this.end[2];
		this.division[0] = value['division_x'] || this.division[0];
		this.division[1] = value['division_y'] || this.division[1];
		this.division[2] = value['division_z'] || this.division[2];
	}

	validate() {
		// TODO: Implement
		return true;
	}
}