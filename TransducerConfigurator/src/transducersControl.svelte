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
            title: "X Position [cm]",
            field: "position_x",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Y Position [cm]",
            field: "position_y",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Z Position [cm]",
            field: "position_z",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "X Target [cm]",
            field: "target_x",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Y Target [cm]",
            field: "target_y",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Z Target [cm]",
            field: "target_z",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Radius [cm]",
            field: "radius",
            sorter: "number",
            sorterParams: {
              alignEmptyValues: "top"
            },
            editor: true
          },
          {
            title: "Phase Shift [rad]",
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
          }
        ]
      });
    });
  });

  function saveTransducers() {
    let result = [];

    tableData.forEach(row => {
      result.push(utils.Transducer.fromTableRowData(row));
    });

    simulationConfiguration.set({
      ...$simulationConfiguration,
      transducers: result
    });
  }

  function addTransducers() {
    let transducer = new utils.Transducer();
    tableData.push(transducer.tableRowData);
  }

  function loadFromClipboard() {
    try {
      let userInput = window.prompt("Paste Here:");

      let parsed = JSON.parse(userInput);

      if (!Array.isArray(parsed)) {
        throw "Root element is not array";
      }

      tableData.splice(0, tableData.length);
      $simulationConfiguration["transducers"] = [];
      parsed.forEach(item => {
        let transducer = new utils.Transducer();
        transducer.data = item;
        tableData.push(transducer.tableRowData);
        $simulationConfiguration["transducers"].push(transducer);
      });
    } catch (e) {
      window.alert(`Failed to load: ${e}`);
    }
  }

  function saveToClipboard() {
    let result = [];

    tableData.forEach(row => {
      result.push(utils.Transducer.fromTableRowData(row));
    });

    let exportResult = JSON.stringify(result);
    console.log(`Transducer configuration: ${exportResult}`);

    navigator.clipboard
      .writeText(exportResult)
      .then(() => {
        window.alert("Copied!");
      })
      .catch(() => {
        window.alert("Failed to copy!");
      });
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
      "output_power"
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

<link href="static/tabulator.min.css" rel="stylesheet" />
<svelte:options tag="transducers-control" />

<div class="container">
  <button on:click={saveTransducers}>Sync to 3D View</button>
  <button on:click={addTransducers}>Add Transducer</button>
  <button on:click={removeTransducers}>Remove Selected Transducers</button>
  <button on:click={multieditor}>Advanced Edit</button>
  <br />
  <button on:click={saveToClipboard}>Copy Transducer Configuration</button>
  <button on:click={loadFromClipboard}>Paste Transducer Configuration</button>
  <div id="transducers-table" bind:this={tableTarget} />
</div>
<slot />
