<script>
  import {
    simulationConfiguration,
    flag_renderSimulationRegion
  } from "./store.js";

  import * as utils from "./utils.js";

  let runningSimulation = false;
  let errorReport = "Loading...";

  // inputData is an object that is binded to the input form
  let inputData = {};

  function toggleSimulationRegionDisplay() {
    flag_renderSimulationRegion.update(val => !val);
  }

  function runSimulation() {
    runningSimulation = true;
    fetch("api/run_computation")
      .then(response => {
        runningSimulation = false;
      })
      .catch(error => {
        runningSimulation = false;
        errorReport = error;
      });
  }

  setTimeout(() => {
    inputData = $simulationConfiguration["simulation_geometry"].flatData;
    errorReport = "";
  }, 1000);

  $: {
    let newSimulationGeometry = utils.SimulationGeometry.fromFlatData(
      inputData
    ).validate();

    if (newSimulationGeometry.isOk) {
      errorReport = "";
      simulationConfiguration.set({
        ...$simulationConfiguration,
        simulation_geometry: newSimulationGeometry.unwrap()
      });
    } else {
      errorReport = `Invalid Geometry: ${newSimulationGeometry.value}`;
    }
  }
</script>

<style>
  .inline {
    display: inline-block;
  }

  .form-container {
    margin: 25px;
    margin-top: 0px;
    display: grid;
    grid-template-columns: auto 4fr;
    grid-gap: 2px 15px;
    grid-auto-columns: auto;
  }

  .error_report {
    margin: 15px;
    color: red;
  }

  .scrollable {
    widows: 100%;
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

  {#if errorReport != ''}
    <p class="error_report">{errorReport}</p>
  {/if}

  <span class="form-container">
    Simulation Plane:
    <select bind:value={inputData['plane']}>
      <option value="X">X</option>
      <option value="Y">Y</option>
      <option value="Z">Z</option>
    </select>
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
    Cell Size:
    <input id="cell_size" bind:value={inputData['cell_size']} />
    Potential Compute Constant 1 (K1):
    <input
      id="potential_compute_const_1"
      bind:value={inputData['potential_compute_const_1']} />
    Potential Compute Constant 2 (K2):
    <input
      id="potential_compute_const_2"
      bind:value={inputData['potential_compute_const_2']} />
    <br />
  </span>

  <button on:click={runSimulation} disabled={runningSimulation}>
    Run Pressure Computation
  </button>
  {#if runningSimulation}
    <p>Computation is Running.</p>
  {/if}
</div>

<slot />
