document.addEventListener("DOMContentLoaded", () => {

	const objects = ["ObjA", "ObjB", "ObjC", "ObjD"];
	const components = [
		{ id: "pos",    label: "Position", cls: "pos" },
		{ id: "vel",    label: "Velocity", cls: "vel" },
		{ id: "sprite", label: "Sprite",   cls: "spr" },
		{ id: "coll",   label: "Collider", cls: "coll" }
	];

	// ---------- Build OOP Layout ----------
	const oopContainer = document.getElementById("oop-memory");

	objARow = createMemoryRow("ObjA");
	oopContainer.appendChild(objARow);

	Reveal.on("fragmentshown", event => {
		const step = event.fragment.dataset.step;

		switch (step) {
			case "add-pos":    addMemoryBox(objARow, components[0]); break;
			case "add-vel":    addMemoryBox(objARow, components[1]); break;
			case "add-sprite": addMemoryBox(objARow, components[2]); break;
			case "add-coll":   addMemoryBox(objARow, components[3]); break;
			case "add-objb":   addFullObject(oopContainer, "ObjB"); break;
			case "add-objc":   addFullObject(oopContainer, "ObjC"); break;
			case "add-objd":   addFullObject(oopContainer, "ObjD"); break;
		}
	});

	function addFullObject(container, name) {
		const row = createMemoryRow(name);

		components.forEach(c => {
			addMemoryBox(row, c);
		});

		container.appendChild(row);
	}

	function createMemoryRow(objectName) {
		const row = document.createElement("div");
		row.className = "memory-row";

		const title = document.createElement("div");
		title.className = "memory-label";
		title.textContent = objectName;

		row.appendChild(title);
		return row;
	}

	function addMemoryBox(row, component, label = component.label) {
		const box = document.createElement("div");
		box.className = `memory-box ${component.cls}`;
		box.dataset.id = component.id;
		box.textContent = label;

		row.appendChild(box);
	}
	
	const oopContainer2 = document.getElementById("oop-memory-2");
	addFullObject(oopContainer2, "ObjA");
	addFullObject(oopContainer2, "ObjB");
	addFullObject(oopContainer2, "ObjC");
	addFullObject(oopContainer2, "ObjD");

	// ---------- Build ECS Layout ----------
	const ecsContainer = document.getElementById("ecs-memory");

	components.forEach(c => {
		const row = createMemoryRow(c.label + "[]");

		objects.forEach(obj => {
			addMemoryBox(row, c, obj);
		});

		ecsContainer.appendChild(row);
	});
});
