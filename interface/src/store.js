import {
	readable,
	writable,
	get
} from 'svelte/store'

export const flag_renderSimulationRegion = writable(false);
export const flag_simulationConfiguration_ServerUpdateLock = writable(true);

export const simulationConfiguration = writable({
	"transducers": [],
	"simulation_geometry": {
		"plane": "X",
		"begin": [0, 0, 0],
		"end": [0, 0, 0],
		"division": [1, 1, 1]
	}
});

export const fetchSimulationConfigurationFromServer = () => {
	flag_simulationConfiguration_ServerUpdateLock.set(true);
	fetch("api/data")
		.then((response) => {
			return response.json();
		})
		.then((value) => {
			simulationConfiguration.set(value);
			flag_simulationConfiguration_ServerUpdateLock.set(false);
		});
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
			})
			.catch(error => console.error(error));
	}
});