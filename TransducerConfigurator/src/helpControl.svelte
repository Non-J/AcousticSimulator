<script>

</script>

<style>
  .padding-15 {
    padding: 15px;
  }

  .scrollable {
    widows: 100%;
    height: 100%;
    overflow: scroll;
  }
</style>

<svelte:options tag="help-control" />
<div class="padding-15 scrollable">
  <h1>Acoustic Simulator - Transducers Configurator UI</h1>
  <p>In Development.</p>

  <h3>Transducers Configurator</h3>
  <ul>
    <li>
      Transducers Configurator is used to place and visualize transducers in 3D
      space.
    </li>
    <li>
      Configuration can be saved and loaded be copying and pasting configuration
      text.
    </li>
    <li>The system expect SI unit, unless noted otherwise.</li>
    <li>
      [Important Notice] The actual simulation computation has been moved to "ComputeEngine".
    </li>
  </ul>

  <h3>Transducers Configurator User Interface</h3>
  <ul>
    <li>
      The User Interface is organized into tabs which are movable and
      resizeable.
    </li>
    <li>Drag tabs to rearrange or stack them.</li>
    <li>
      [Known Issue] When the tab system is loaded, sometimes the tab get hidden
      in a menu indicated by a down array in the tab bar.
    </li>
  </ul>

  <h3>Transducers Tab</h3>
  <ul>
    <li>The transducer tab shows a table of all transducers.</li>
    <li>Each of the row represent a single transducer.</li>

    <li>Each row and column can be reordered and resized.</li>
    <li>To add, use "Add Transducer" option.</li>
    <li>To edit, click on the desired value and make changes.</li>
    <li>
      To remove, select the desired transducer using tickbox and use "Remove
      Selected Transducers" option.
    </li>
    <li>
      To use "Advanced Edit" option, select the desired transducer using
      tickbox, click the option to start editing, and follow the guided
      instructions.
    </li>
    <li>Use "Sync to 3DView" to update 3DView</li>
    <li>
      Use "Copy Transducer Configuration" to copy transducers which can be used
      in ComputeEngine or save to text file.
    </li>
    <li>
      Use "Paste Transducer Configuration" to load transducers into the program.
    </li>
    <li>
      [Known Issue] The copy/paste feature uses experimental feature of the
      browser which may not work properly.
    </li>
  </ul>

  <h3>3DView Tab</h3>
  <p>
    The 3DView shows the current simulation configuration.
    <br />
    The grid and axes help user to orient in 3D space. The center pointer
    indicates the axes:
  </p>
  <ul>
    <li>X-axis is red</li>
    <li>Y-axis is green</li>
    <li>Z-axis is blue</li>
  </ul>
  <p>
    The camera uses "Orbital Control", which means that it always looks to some
    invisible center point. Click inside the 3DView to activate the control:
  </p>
  <ul>
    <li>Click+Drag moves the camera around.</li>
    <li>
      Scroll Up/Down to zoom out/in. (i.e: Changes the distance to the center
      point)
    </li>
    <li>Use arrow keys to move the center point.</li>
    <li>Press 'R' to reset the camera to its initial state.</li>
  </ul>

  <h3>Simulation Tab</h3>
  <ul>
    <li>Visualize the region that will be simulated.</li>
    <li>Toggle Simulation Region On/Off to display in 3DView.</li>
    <li>
      [Important Notice] Parameters here are for visualization only. The actual
      parameters are controlled in ComputeEngine.
    </li>
    <li>
      [Known Issue] Values here are not scaled correctly. It is up to the user to set the correct scale in ComputeEngine.
    </li>
  </ul>

  <br />
</div>
<slot />
