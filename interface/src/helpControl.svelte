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
  <h1>Acoustic Simulator</h1>
  <p>In Development.</p>

  <h3>Simulator</h3>
  <p>
    The simulator computes pressure generated from multiple transducers.
    <br />
    The computation is unit-independent. As long as unit used for all quantity
    are consistent, the output will also be consistent with the unit.
  </p>

  <h3>User Interface</h3>
  <p>
    The User Interface is organized into tabs which are movable and resizeable.
    <br />
    Drag tabs to rearrange or stack them.
  </p>

  <h3>3DView</h3>
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

  <h3>Simulation</h3>
  <p>
    Simulation tab hosts control of the simulator. This is also where you start the computation.
    <br />
    Toggle Simulation Region On/Off to visualize the region that will be
    simulated.
    <br />
    The Simulation Plane is the plane at which the output will divided into:
  </p>
  <ul>
    <li>
      X: Output files for each of X-value. Row is Y-value, Column is Z-value.
    </li>
    <li>
      Y: Output files for each of Y-value. Row is X-value, Column is Z-value.
    </li>
    <li>
      Z: Output files for each of Z-value. Row is Y-value, Column is X-value.
    </li>
  </ul>
  <p>
    Simulation Begin/End determines the region which is being simulated.
    Division count determines the resolution.
  </p>

  <h3>Transducers</h3>
  <p>
    The transducer tab shows a table of all transducers.
    <br />
    For changes made to take effect, 
  </p>

  <h3>Known Problems and Potential Work Arounds</h3>
  <ul>
    <li>In general, try to reload the page if a problem occurs.</li>
    <li>
      This interface relies on your web browser to properly support the latest
      web standard and technology. Firefox and Google Chrome both highly
      comforms to the web standard and technology.
    </li>
    <li>
      If the transducer table is empty, click on the table header to refresh it.
    </li>
    <li>
      Data validation is not complete. Invalid simulation configulation may
      result in incorrect result.
      <br />
      Example: Having 2 transducers intersect with each other.
    </li>
    <li>
      Tab system may sometimes be hidden in menu, which are indicated by a tiny
      triangle on the tab bars.
    </li>
    <li>
      3DView requires support of WebGL technology. Some browser or computer may
      not support them properly which results in black screen, erroneous 3D
      geometry, or system crash.
    </li>

  </ul>

  <br />
</div>
<slot />
