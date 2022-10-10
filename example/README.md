# DIS-PIPE User Guide
In the following we provide a detailed guide of the various steps required to execute DIS-PIPE and interact with its graphical user interface (GUI). For the sake of understandability, we evaluate the interaction with the GUI by relying on a real-world data pipeline provided by JOT, one of the business case partners in the project (e.g., the JOT pipeline involves the management of digital marketing campaigns). The event log passed as input to the tool can be visualised in three alternative ways, relying on the **Map** view, the **Event Log Analysis** view, or the **Conformance Checking** view. Since the current version of the tool does not still implement the Segmentation, Preprocessing and Event Abstraction functionalities, we suppose that the input event log is already well-structured to represent the single execution traces related to the past pipeline executions. DIS-PIPE provides a user-friendly interface for the automated analysis of an event log through interactive visualisations that allow decision makers to combine their flexibility, creativity, and background knowledge to come to an effective understanding of situations in the context of large data sets. Adopting three different visual perspectives of the same data is in line with the literature solutions to visualise the results of a process mining activity, where human judgment is essential in finding interesting and relevant patterns.

## The Map View

The Map View is the first visualisation shown by default when DIS-PIPE is invoked and can be described by looking at its main components:

![alt text](https://raw.githubusercontent.com/DataCloud-project/DIS-PIPE/main/example/images/DIS-PIPE-Map-View.png)

**(i) Log name**: the name of the log file from which the pipeline model is extracted is shown in the top part of the GUI.

**(ii) Canvas with Pipeline Map**: DIS-PIPE automatically runs the implemented discovery algorithm and produces an understandable flowchart view of the discovered pipeline in the form of a Directly-Follows Graph (DFG).

**(iii) Zoom Slider**: gives the users explicit control to make the pipeline map larger and smaller. Alternatively, it can be simply done using the mouse wheel to zoom in and out. The currently displayed area of the map can be moved around by clicking and holding the mouse while dragging the map.

**(iv) Pipeline Map Visualization Options**: since real-life data pipelines can become quite complex and confusing when every detail and all exceptional flows are shown, DIS-PIPE gives you a quick and easy way to simplify the map and only show you the most important flows. Two slider controls can be used to modify the level of detail that is shown in the map: the **step slider** determines the minimum number of traces in which a step needs to appear in the log to be shown in the map. In contrast, the **path slider** determines the percentage of different traces that need to be showed on the map, removing the least frequent ones. by default, the **Frequency tab** is selected, and the pipeline map is displayed with absolute frequencies. This means that the numbers in the steps and at the paths indicate how many times the step was performed in total or how often that path has been “travelled” throughout the pipeline. Users can also enhance the discovered maps with other frequency metrics (absolute frequency, case frequency and maximum repetitions). After obtaining a good understanding of the actual pipeline flow, it is often interesting to know more about the time spent in its different parts. The performance metrics can be used to enhance the map by selecting the **Performance tab**, which allows the choice between different values (total, median, mean, minimum and maximum duration). Selecting an element of the DFG by clicking on it, a pop-up shows all the metrics related to the selected tab at once, and multiple selections can be done to analyse paths.

**(v) View Switcher**: it allows to switch the view of DIS-PIPE among the **Map** view, the **Event Log Analysis** view, or the **Conformance Checking** view.

**(vi) I/O Options**: it allows to import a XES event log within DIS-PIPE or export a DSL model of a data pipeline locally (or remotely).  

## The Event Log Analysis View

While the Map view gives an understanding of the pipeline flow, the Event Log Analysis view goes down to the individual case level, shows the raw data and allows the possibility to apply different kinds of filters. To inspect individual cases is important to verify findings and see concrete examples of “strange” behavior that can be discovered during the pipeline analysis. Often, things that are hard to believe can be analysed by drilling them down to a particular example case, noting their case number, and verifying that this is indeed what happened in the system(s) under analysis. Furthermore, looking at individual cases with their history and attributes can give additional context (like a comment field) that sometimes explains why something happened. 

Finally, the ability to drill down to individual cases is important to act on an analysis. For example, it can be used to find deviations from the described pipelines or violations of an important business rule, get a list of these cases, and prepare a fix.

This view can be divided into three main areas, as can be seen in the following figure:

![alt text](https://raw.githubusercontent.com/DataCloud-project/DIS-PIPE/main/example/images/DIS-PIPE-Event-Log-Analysis.png)
