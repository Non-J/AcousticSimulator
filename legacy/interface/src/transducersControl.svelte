<script>
  import * as tabulator from "tabulator-tables";

  import isEqual from "lodash/isEqual";
  import find from "lodash/find";
  import remove from "lodash/remove";

  import { simulationConfiguration } from "./store.js";
  import * as utils from "./utils.js";

  import { onMount, onDestroy } from "svelte";

  let table;
  let tableTarget;
  let tableData = [];

  onMount(() => {
    setTimeout(() => {
      $simulationConfiguration["transducers"].forEach(transducer => {
        tableData.push(transducer.tableRowData);
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
    let saveFlag = true;
    let result = [];

    tableData.forEach(row => {
      if (!saveFlag) {
        return;
      }

      let newTransducer = utils.Transducer.fromTableRowData(row).validate();
      if (newTransducer.isOk) {
        result.push(newTransducer.unwrap());
      } else {
        alert(`Invalid Transducer ${row.id}: ${newTransducer.value}`);
        saveFlag = false;
      }
    });

    if (saveFlag) {
      simulationConfiguration.set({
        ...$simulationConfiguration,
        transducers: result
      });
    }
  }

  function addTransducers() {
    let transducer = new utils.Transducer();
    tableData.push(transducer.tableRowData);
  }

  function removeTransducers() {
    let selected = table.getSelectedData();
    for (let i = 0; i < tableData.length; i++) {
      if (
        find(
          selected,
          (val, idx, col) => val["row_id"] === tableData[i]["row_id"]
        ) !== undefined
      ) {
        tableData.splice(i, 1);
        i--;
      }
    }
  }

  function multieditor() {
    let selected = table.getSelectedData();

    let editFields = [
      "position_x",
      "position_y",
      "position_z",
      "target_x",
      "target_y",
      "target_z",
      "radius",
      "phase_shift",
      "loss_factor",
      "output_power",
      "wavelength"
    ];
    let newValues = {};

    for (let field of editFields) {
      let query = window.prompt(
        `Enter new ${field} value (Leave empty for unchange)`
      );
      if (query !== "" && !Number.isNaN(Number(query))) {
        newValues[field] = Number(query);
      }
    }

    for (let i = 0; i < tableData.length; i++) {
      if (
        find(selected, (val, idx, col) => isEqual(val, tableData[i])) !==
        undefined
      ) {
        for (let field in newValues) {
          tableData[i][field] = newValues[field];
        }
      }
    }
  }
</script>

<style>
  .container {
    width: 100%;
    height: 100%;
    overflow: scroll;
  }
</style>

<link href="tabulator.min.css" rel="stylesheet" />
<svelte:options tag="transducers-control" />

<div class="container">
  <button on:click={saveTransducers}>Save Transducers</button>
  <button on:click={addTransducers}>Add Transducers</button>
  <button on:click={removeTransducers}>Remove Selected Transducers</button>
  <button on:click={multieditor}>Multi Edit</button>
  <div id="transducers-table" bind:this={tableTarget} />
</div>
<slot />
