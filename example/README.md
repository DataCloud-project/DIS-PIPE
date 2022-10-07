# DIS-PIPE User Guide
In the following we provide a detailed guide of the various steps required to execute DIS-PIPE and interact with its graphical user interface (GUI). For the sake of understandability, we evaluate the interaction with the GUI by relying on a real-world data pipeline provided by JOT, one of the business case partners in the project (e.g., the JOT pipeline involves the management of digital marketing campaigns). The event log passed as input to the tool can be visualised in three alternative ways, relying on the Map view, the Event Log Analysis view, or the Conformance Checking view. Since the current version of the tool does not still implement the Segmentation, Preprocessing and Event Abstraction functionalities, we suppose that the input event log is already well-structured to represent the single execution traces related to the past pipeline executions. DIS-PIPE provides a user-friendly interface for the automated analysis of an event log through interactive visualisations that allow decision makers to combine their flexibility, creativity, and background knowledge to come to an effective understanding of situations in the context of large data sets. Adopting three different visual perspectives of the same data is in line with the literature solutions to visualise the results of a process mining activity, where human judgment is essential in finding interesting and relevant patterns.

## The Map View

The Map View is the first visualisation shown by default when DIS-PIPE is invoked and can be described by looking at its main components:

![alt text](https://raw.githubusercontent.com/DataCloud-project/DIS-PIPE/main/example/images/DIS-PIPE-Map-View.png)

**(i) Log name**: the name of the log file from which the pipeline model is extracted is shown in the top part of the GUI.

**(ii) Canvas with Pipeline Map**: DIS-PIPE automatically runs the implemented discovery algorithm and produces an understandable flowchart view of the discovered pipeline in the form of a Directly-Follows Graph (DFG).

**(iii) Zoom Slider**: gives the users explicit control to make the pipeline map larger and smaller. Alternatively, it can be simply done using the mouse wheel to zoom in and out. The currently displayed area of the map can be moved around by clicking and holding the mouse while dragging the map.

**(iv) Pipeline Map Visualization Options**: since real-life data pipelines can become quite complex and confusing when every detail and all exceptional flows are shown, DIS-PIPE gives you a quick and easy way to simplify the map and only show you the most important flows.

**(v) View Switcher**: it allows to change the view of DIS-PIPE.

**(vi) I/O Options**: it allows to import a XES event log within DIS-PIPE. 
