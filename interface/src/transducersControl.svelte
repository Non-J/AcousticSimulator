<script>
  import * as tabulator from "tabulator-tables";

  import isEqual from "lodash/isEqual";
  import find from "lodash/find";
  import remove from "lodash/remove";

  import { simulationConfiguration } from "./store.js";

  import { onMount, onDestroy } from "svelte";

  let table;
  let tableTarget;
  let tableData = [];

  onMount(() => {
    setTimeout(() => {
      $simulationConfiguration["transducers"].forEach(transducer => {
        tableData.push({
          ...transducer,
          position_x: transducer["position"][0],
          position_y: transducer["position"][1],
          position_z: transducer["position"][2],
          target_x: transducer["target"][0],
          target_y: transducer["target"][1],
          target_z: transducer["target"][2],
          rid: Math.random()
        });
      });
      table = new tabulator.default(tableTarget, {
        movableRows: true,
        movableColumns: true,
        selectable: true,
        reactiveData: true,
        layout: "fitDataStretch",
        data: tableData,
        columns: [
          {
            formatter: "rowSelection",
            titleFormatter: "rowSelection",
            align: "center",
            headerSort: false,
            cellClick: function(err, cell) {
              cell.getRow().toggleSelect();
            }
          },
          { title: "Name", field: "id", editor: true },
          {
            title: "X Position",
            field: "position_x",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Y Position",
            field: "position_y",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Z Position",
            field: "position_z",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "X Target",
            field: "target_x",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Y Target",
            field: "target_y",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Z Target",
            field: "target_z",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Radius",
            field: "radius",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Phase Shift",
            field: "phase_shift",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Loss Factor",
            field: "loss_factor",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Output Power",
            field: "output_power",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Wavelength",
            field: "wavelength",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          }
        ]
      });
    }, 1000);
  });

  function saveTransducers() {
    let result = [];
    tableData.forEach(row => {
      result.push({
        id: row["id"],
        radius: Number(row["radius"]),
        phase_shift: Number(row["phase_shift"]),
        loss_factor: Number(row["loss_factor"]),
        output_power: Number(row["output_power"]),
        wavelength: Number(row["wavelength"]),
        position: [
          Number(row["position_x"]),
          Number(row["position_y"]),
          Number(row["position_z"])
        ],
        target: [
          Number(row["target_x"]),
          Number(row["target_y"]),
          Number(row["target_z"])
        ]
      });
    });
    simulationConfiguration.set({
      ...$simulationConfiguration,
      transducers: result
    });
  }

  function addTransducers() {
    tableData.push({
      id: "",
      position_x: 0.0,
      position_y: 0.0,
      position_z: 0.0,
      target_x: 0.0,
      target_y: 1.0,
      target_z: 0.0,
      radius: 0.5,
      phase_shift: 0.0,
      loss_factor: 1.0,
      output_power: 1.0,
      wavelength: 8.0,
      rid: Math.random(),
    });
  }

  function removeTransducers() {
    let selected = table.getSelectedData();
    for (let i = 0; i < tableData.length; i++) {
      if (
        find(selected, (val, idx, col) => isEqual(val, tableData[i])) !==
        undefined
      ) {
        tableData.splice(i, 1);
        i--;
      }
    }
  }
</script>

<link href="tabulator.min.css" rel="stylesheet" />

<svelte:options tag="transducers-control" />
<button on:click={saveTransducers}>Save Transducers</button>
<button on:click={addTransducers}>Add Transducers</button>
<button on:click={removeTransducers}>Remove Selected Transducers</button>
<div id="transducers-table" bind:this={tableTarget} />
<slot />
