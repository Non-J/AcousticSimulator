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

export function vectorStringToNumber(value) {
	if (Array.isArray(value)) {
		let result = value.map(i => Number(i));
		for (let i of result) {
			if (Number.isNaN(i)) {
				return null;
			}
		}
		return result;
	}
	return null;
}

export class Transducer {
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
		this.position[0] = parseNumber(value['position_x']).unwrap_or(this.position[0]);
		this.position[1] = parseNumber(value['position_y']).unwrap_or(this.position[1]);
		this.position[2] = parseNumber(value['position_z']).unwrap_or(this.position[2]);
		this.target[0] = parseNumber(value['target_x']).unwrap_or(this.target[0]);
		this.target[1] = parseNumber(value['target_y']).unwrap_or(this.target[1]);
		this.target[2] = parseNumber(value['target_z']).unwrap_or(this.target[2]);
		this.radius = parseNumber(value['radius']).unwrap_or(this.radius);
		this.phase_shift = parseNumber(value['phase_shift']).unwrap_or(this.phase_shift);
		this.loss_factor = parseNumber(value['loss_factor']).unwrap_or(this.loss_factor);
		this.output_power = parseNumber(value['output_power']).unwrap_or(this.output_power);
		this.wavelength = parseNumber(value['wavelength']).unwrap_or(this.wavelength);
	}

	get data() {
		return {
			id: this.id,
			position: this.position,
			target: this.target,
			radius: this.radius,
			phase_shift: this.phase_shift,
			loss_factor: this.loss_factor,
			output_power: this.output_power,
			wavelength: this.wavelength,
		}
	}

	set data(value) {
		this.id = value['id'] || this.id;
		this.position = vectorStringToNumber(value['position']) || this.position;
		this.target = vectorStringToNumber(value['target']) || this.target;
		this.radius = parseNumber(value['radius']).unwrap_or(this.radius);
		this.phase_shift = parseNumber(value['phase_shift']).unwrap_or(this.phase_shift);
		this.loss_factor = parseNumber(value['loss_factor']).unwrap_or(this.loss_factor);
		this.output_power = parseNumber(value['output_power']).unwrap_or(this.output_power);
		this.wavelength = parseNumber(value['wavelength']).unwrap_or(this.wavelength);
	}

	static from(value) {
		let newTransducer = new Transducer();
		newTransducer.data = value;
		return newTransducer;
	}

	static fromTableRowData(value) {
		let newTransducer = new Transducer();
		newTransducer.tableRowData = value;
		return newTransducer;
	}

	validate() {
		// TODO: Implement
		return true;
	}
}

export class SimulationGeometry {
	constructor() {
		this.plane = "X";
		this.begin = [-10, 0, -10];
		this.end = [10, 0, 10];
		this.division = [250, 1, 250];
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
		this.begin[0] = parseNumber(value['begin_x']).unwrap_or(this.begin[0]);
		this.begin[1] = parseNumber(value['begin_y']).unwrap_or(this.begin[1]);
		this.begin[2] = parseNumber(value['begin_z']).unwrap_or(this.begin[2]);
		this.end[0] = parseNumber(value['end_x']).unwrap_or(this.end[0]);
		this.end[1] = parseNumber(value['end_y']).unwrap_or(this.end[1]);
		this.end[2] = parseNumber(value['end_z']).unwrap_or(this.end[2]);
		this.division[0] = parseNumber(value['division_x']).unwrap_or(this.division[0]);
		this.division[1] = parseNumber(value['division_y']).unwrap_or(this.division[1]);
		this.division[2] = parseNumber(value['division_z']).unwrap_or(this.division[2]);
	}


	get data() {
		return {
			plane: this.plane,
			begin: this.begin,
			end: this.end,
			division: this.division,
		}
	}

	set data(value) {
		this.plane = value['plane'] || this.plane;
		this.begin = vectorStringToNumber(value['begin']) || this.begin;
		this.end = vectorStringToNumber(value['end']) || this.end;
		this.division = vectorStringToNumber(value['division']) || this.division;
	}

	static from(value) {
		let newSimulationGeometry = new SimulationGeometry();
		newSimulationGeometry.data = value;
		return newSimulationGeometry;
	}

	static fromFlatData(value) {
		let newSimulationGeometry = new SimulationGeometry();
		newSimulationGeometry.flatData = value;
		return newSimulationGeometry;
	}

	get cubicSize() {
		// Number.MIN_VALUE is added as THREE.js treat box of size 0 as default of 1
		return [
			Math.abs(this.end[0] - this.begin[0]) + Number.MIN_VALUE,
			Math.abs(this.end[1] - this.begin[1]) + Number.MIN_VALUE,
			Math.abs(this.end[2] - this.begin[2]) + Number.MIN_VALUE,
		];
	}

	get cubicPosition() {
		return [
			(this.end[0] + this.begin[0]) / 2.0,
			(this.end[1] + this.begin[1]) / 2.0,
			(this.end[2] + this.begin[2]) / 2.0,
		];
	}

	validate() {
		// TODO: Implement
		return new Result(true, this);
	}
}