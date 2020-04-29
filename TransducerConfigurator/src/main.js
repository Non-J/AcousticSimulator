import './jquery-globals.js';
import GoldenLayout from 'golden-layout';

let layoutManager = new GoldenLayout({
	settings: {
		showPopoutIcon: false,
		showMaximiseIcon: false,
		showCloseIcon: false
	},
	content: [{
		type: 'row',
		content: [{
			type: 'stack',
			content: [{
				type: 'component',
				componentName: 'help-control',
				componentState: {
					label: 'Help'
				},
				isClosable: false,
				title: "Help",
			}, {
				type: 'component',
				componentName: 'simulation-control',
				componentState: {
					label: 'Simulation'
				},
				isClosable: false,
				title: "Simulation",
			}, {
				type: 'component',
				componentName: 'transducers-control',
				componentState: {
					label: 'Transducers'
				},
				isClosable: false,
				title: "Transducers",
			}]
		}, {
			type: 'component',
			componentName: 'renderer-control',
			componentState: {
				label: '3D View'
			},
			isClosable: false,
			title: "3D View",
		}, ]
	}]
});

import './helpControl.svelte';
layoutManager.registerComponent('help-control', function (container, componentState) {
	let transducersControlElement = document.createElement("help-control");
	container.getElement().get(0).appendChild(transducersControlElement);
});

import './simulationControl.svelte';
layoutManager.registerComponent('simulation-control', function (container, componentState) {
	let simulationControlElement = document.createElement("simulation-control");
	container.getElement().get(0).appendChild(simulationControlElement);
});

import './transducersControl.svelte';
layoutManager.registerComponent('transducers-control', function (container, componentState) {
	let transducersControlElement = document.createElement("transducers-control");
	container.getElement().get(0).appendChild(transducersControlElement);
});

import './rendererControl.svelte';
layoutManager.registerComponent('renderer-control', function (container, componentState) {
	let rendererElement = document.createElement("renderer-control");
	rendererElement.setAttribute("width", String(container.width));
	rendererElement.setAttribute("height", String(container.height));
	container.on("resize", (e) => {
		rendererElement.setAttribute("width", String(container.width));
		rendererElement.setAttribute("height", String(container.height));
	});
	container.getElement().get(0).appendChild(rendererElement);
});

layoutManager.init();

import App from './app.svelte';

const app = new App({
	target: document.getElementById("svelte-app"),
});

export default app;