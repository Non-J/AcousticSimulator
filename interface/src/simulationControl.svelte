<script>
  import {
    simulationConfiguration,
    flag_renderSimulationRegion
  } from "./store.js";

  let runningSimulation = false;
  let errorReport = "Loading...";
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
    inputData = {
      plane: $simulationConfiguration["simulation_geometry"]["plane"],
      begin_x: $simulationConfiguration["simulation_geometry"]["begin"][0],
      begin_y: $simulationConfiguration["simulation_geometry"]["begin"][1],
      begin_z: $simulationConfiguration["simulation_geometry"]["begin"][2],
      end_x: $simulationConfiguration["simulation_geometry"]["end"][0],
      end_y: $simulationConfiguration["simulation_geometry"]["end"][1],
      end_z: $simulationConfiguration["simulation_geometry"]["end"][2],
      cnt_x: $simulationConfiguration["simulation_geometry"]["division"][0],
      cnt_y: $simulationConfiguration["simulation_geometry"]["division"][1],
      cnt_z: $simulationConfiguration["simulation_geometry"]["division"][2]
    };
    errorReport = "";
  }, 1000);

  $: {
    if (Number.isNaN(Number(inputData["begin_x"]))) {
      errorReport = "Begin X is not a valid number.";
    } else if (Number.isNaN(Number(inputData["begin_y"]))) {
      errorReport = "Begin Y is not a valid number.";
    } else if (Number.isNaN(Number(inputData["begin_z"]))) {
      errorReport = "Begin Z is not a valid number.";
    } else if (Number.isNaN(Number(inputData["end_x"]))) {
      errorReport = "End X is not a valid number.";
    } else if (Number.isNaN(Number(inputData["end_y"]))) {
      errorReport = "End Y is not a valid number.";
    } else if (Number.isNaN(Number(inputData["end_z"]))) {
      errorReport = "End Z is not a valid number.";
    } else if (
      !Number.isInteger(Number(inputData["cnt_x"])) ||
      Number(inputData["cnt_x"]) < 1
    ) {
      errorReport = "Division X is not a valid number or is not an integer.";
    } else if (
      !Number.isInteger(Number(inputData["cnt_y"])) ||
      Number(inputData["cnt_y"]) < 1
    ) {
      errorReport = "Division Y is not a valid number or is not an integer.";
    } else if (
      !Number.isInteger(Number(inputData["cnt_z"])) ||
      Number(inputData["cnt_z"]) < 1
    ) {
      errorReport = "Division Z is not a valid number or is not an integer.";
    } else {
      errorReport = "";
      simulationConfiguration.set({
        ...$simulationConfiguration,
        simulation_geometry: {
          plane: inputData["plane"],
          begin: [
            Number(inputData["begin_x"]),
            Number(inputData["begin_y"]),
            Number(inputData["begin_z"])
          ],
          end: [
            Number(inputData["end_x"]),
            Number(inputData["end_y"]),
            Number(inputData["end_z"])
          ],
          division: [
            Number(inputData["cnt_x"]),
            Number(inputData["cnt_y"]),
            Number(inputData["cnt_z"])
          ]
        }
      });
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
    Division X:
    <input id="cnt_x" bind:value={inputData['cnt_x']} />
    Division Y:
    <input id="cnt_y" bind:value={inputData['cnt_y']} />
    DIvision Z:
    <input id="cnt_z" bind:value={inputData['cnt_z']} />
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
