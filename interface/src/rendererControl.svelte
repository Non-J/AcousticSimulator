<script>
  export let width = null;
  export let height = null;

  import {
    simulationConfiguration,
    flag_renderSimulationRegion
  } from "./store.js";

  import * as utils from "./utils.js";

  import { onMount, onDestroy } from "svelte";
  import * as THREE from "three";
  import { OrbitControls } from "three/examples/jsm/controls/OrbitControls.js";

  // Scene Setups
  let scene = new THREE.Scene();
  scene.autoUpdate = true;
  scene.matrixAutoUpdate = true;
  scene.background = new THREE.Color("gray");

  let gridHelper = new THREE.GridHelper(1000, 1000);
  gridHelper.position.set(0, -0.005, 0);
  scene.add(gridHelper);

  let axesHelper = new THREE.AxesHelper(2.5);
  scene.add(axesHelper);

  let camera = new THREE.PerspectiveCamera();
  let controls, renderer, animationID, rendererCanvas;

  // Rendering and controls
  // Initialized after DOM is ready
  onMount(() => {
    renderer = new THREE.WebGLRenderer({
      antialias: true,
      canvas: rendererCanvas
    });

    controls = new OrbitControls(camera, rendererCanvas);
    controls.keyPanSpeed = 12;
    camera.position.set(10, 10, 10);
    controls.update();
    controls.saveState();

    let animate = () => {
      animationID = requestAnimationFrame(animate);

      renderer.render(scene, camera);
    };
    animate();
  });

  // Layout scaling update
  $: {
    let parsedWidth = utils.parseNumber(width).unwrap_or(500);
    let parsedHeight = utils.parseNumber(height).unwrap_or(500);
    camera.aspect = parsedWidth / parsedHeight;
    camera.updateProjectionMatrix();
    if (renderer) {
      renderer.setSize(parsedWidth, parsedHeight);
    }
  }

  // Simulation region drawing
  let activeSimulationRegionMesh;
  let simulationRegionMesh;

  $: {
    if (activeSimulationRegionMesh) {
      scene.remove(activeSimulationRegionMesh);
      activeSimulationRegionMesh = null;
    }
    activeSimulationRegionMesh = simulationRegionMesh;
    if ($flag_renderSimulationRegion) {
      if (activeSimulationRegionMesh) {
        scene.add(activeSimulationRegionMesh);
      }
    }
  }

  // Drawing
  let transducerCones = [];

  $: {
    let config = $simulationConfiguration;

    // Simulation Region
    let simulationRegionmaterial = new THREE.MeshBasicMaterial({
      color: 0xcfd8dc,
      opacity: 0.25,
      transparent: true,
      side: THREE.DoubleSide
    });

    let simulationRegionGeometry = new THREE.BoxGeometry(
      ...config["simulation_geometry"].cubicSize
    );
    simulationRegionMesh = new THREE.Mesh(
      simulationRegionGeometry,
      simulationRegionmaterial
    );
    simulationRegionMesh.position.set(
      ...config["simulation_geometry"].cubicPosition
    );

    // First, remove the existing mesh
    transducerCones.forEach(cone => {
      scene.remove(cone);
    });

    if (config["transducers"]) {
      transducerCones = [];

      config["transducers"].forEach(transducer => {
        let geometry = new THREE.ConeGeometry(
          transducer["radius"],
          transducer["radius"],
          20,
          20
        );
        let material = new THREE.MeshBasicMaterial({ color: 0x1976d2 });
        let cone = new THREE.Mesh(geometry, material);
        cone.position.set(...transducer["position"]);
        cone.lookAt(...transducer["target"]);
        cone.rotateX(-Math.PI / 2);
        transducerCones.push(cone);
      });
    }

    // Last, add each mesh back
    transducerCones.forEach(cone => {
      scene.add(cone);
    });
  }

  // Key press events
  let handleKeypress = event => {
    switch (event.key) {
      case "r":
      case "R":
        controls.reset();
      default:
        break;
    }
  };

  // Cleanup Code
  onDestroy(() => {
    cancelAnimationFrame(animationID);
    controls = controls.dispose();
  });
</script>

<svelte:options tag="renderer-control" />
<canvas
  bind:this={rendererCanvas}
  style="width:100%; height:100%;"
  on:keypress={handleKeypress} />
<slot />
