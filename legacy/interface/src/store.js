import {
	readable,
	writable,
	get
} from 'svelte/store'

import * as utils from "./utils.js";

export const flag_renderSimulationRegion = writable(false);
export const flag_simulationConfiguration_ServerUpdateLock = writable(true);

export const simulationConfiguration = writable({
	"transducers": [],
	"simulation_geometry": new utils.SimulationGeometry(),
});

export const fetchSimulationConfigurationFromServer = () => {
	flag_simulationConfiguration_ServerUpdateLock.set(true);
	fetch("api/data")
		.then((response) => {
			return response.json();
		})
		.then((value) => {
			let transducers = [];
			value['transducers'].forEach((transducer) => {
				transducers.push(utils.Transducer.from(transducer));
			});
			simulationConfiguration.set({
				'transducers': transducers,
				'simulation_geometry': utils.SimulationGeometry.from(value['simulation_geometry']),
			});
			flag_simulationConfiguration_ServerUpdateLock.set(false);
		});
}

let displaySaveErrorFlag = true;
let displaySaveErrorTimeout;

function displaySaveError(message, timeout = 5000) {
	if (displaySaveErrorFlag) {
		alert(message);
		displaySaveErrorFlag = false;
		if (!displaySaveErrorTimeout) {
			displaySaveErrorTimeout = setTimeout(() => {
				displaySaveErrorFlag = true;
				displaySaveErrorTimeout = null;
			}, timeout);
		}
	}
}

fetchSimulationConfigurationFromServer();
simulationConfiguration.subscribe(value => {
	if (!get(flag_simulationConfiguration_ServerUpdateLock)) {
		fetch("api/data", {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json',
				},
				body: JSON.stringify(value),
			}).then((response) => {
				if (!response.ok) {
					displaySaveError("Saving Configuration Failed.")
				}
			})
			.catch(error => alert(`${error}`));
	}
});