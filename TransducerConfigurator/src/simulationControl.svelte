<script>
  import {
    simulationConfiguration,
    flag_renderSimulationRegion,
    global_linear_distance_scale
  } from "./store.js";

  import * as utils from "./utils.js";

  // inputData is an object that is binded to the input form
  let inputData = {};

  function toggleSimulationRegionDisplay() {
    flag_renderSimulationRegion.update(val => !val);
  }

  // Initial loading
  setTimeout(() => {
    inputData = $simulationConfiguration["simulation_geometry"].flatData;
  });

  // Overwrite save
  $: {
    simulationConfiguration.set({
      ...$simulationConfiguration,
      simulation_geometry: utils.SimulationGeometry.fromFlatData(inputData)
    });
  }
</script>

<style>
  .inline {
    display: inline-block;
  }

  .form-container {
    margin-right: 25px;
    display: grid;
    grid-template-columns: auto 4fr;
    grid-gap: 2px 15px;
    grid-auto-columns: auto;
  }

  .scrollable {
    margin: 1em;
    width: 100%;
    height: 100%;
    overflow: scroll;
  }
</style>

<svelte:options tag="simulation-control" />
<div class="scrollable">
  <h3 class="inline">
    Display Simulation Region: {$flag_renderSimulationRegion ? 'On' : 'Off'}
  </h3>
  <button class="inline" on:click={toggleSimulationRegionDisplay}>
    Toggle
  </button>

  <br />

  <span class="form-container">
    <br />
    Begin X:
    <input id="begin_x" bind:value={inputData['begin_x']} />
    <br />
    Begin Y:
    <input id="begin_y" bind:value={inputData['begin_y']} />
    <br />
    Begin Z:
    <input id="begin_z" bind:value={inputData['begin_z']} />
    <br />
    End X:
    <input id="end_x" bind:value={inputData['end_x']} />
    <br />
    End Y:
    <input id="end_y" bind:value={inputData['end_y']} />
    <br />
    End Z:
    <input id="end_z" bind:value={inputData['end_z']} />
    <br />
    Linear Distance Scale Factor: {global_linear_distance_scale}
    <br />
  </span>

  <p>
    Note: "Linear Distance Scale Factor" determines the visually displayed unit.
    All internal data expects SI unit, so values are scaled to make them easier
    to work with.
  </p>
  <p>
    Note: This is for visualization only. These parameters are controlled by the
    new ComputeEngine.
  </p>

</div>

<slot />
