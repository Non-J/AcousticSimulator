import {
	writable
} from 'svelte/store'

export const flag_renderSimulationRegion = writable(false);

export const simulationConfiguration = writable({
	"transducers": [],
	"simulation_geometry": {},
});

// Scale all display (data must always be SI)
export const global_linear_distance_scale = 100;