package segmentation;

import java.awt.Color;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.Vector;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.text.StyleConstants;

import org.deckfour.xes.extension.std.XConceptExtension;
import org.deckfour.xes.extension.std.XLifecycleExtension;
import org.deckfour.xes.extension.std.XTimeExtension;
import org.deckfour.xes.factory.XFactory;
import org.deckfour.xes.factory.XFactoryRegistry;
import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;
import org.processmining.models.connections.GraphLayoutConnection;
import org.processmining.models.graphbased.directed.petrinet.Petrinet;
import org.processmining.models.graphbased.directed.petrinet.PetrinetEdge;
import org.processmining.models.graphbased.directed.petrinet.PetrinetNode;
import org.processmining.models.graphbased.directed.petrinet.elements.Place;
import org.processmining.models.graphbased.directed.petrinet.elements.Transition;
import org.processmining.models.graphbased.directed.petrinet.impl.PetrinetFactory;
import org.processmining.models.semantics.petrinet.Marking;
import org.processmining.plugins.pnml.Pnml;

public class Main {

	static Properties prop = new Properties();
	static InputStream input = null;
	final static String propertiesFilePath = "app.properties";
	final static Logger LOGGER =  Logger.getLogger(Main.class.getName());
	
	public static final String INVISIBLE_TRANSITION_PREFIX = "generatedINV";
	public static final String WINDOWS = "windows";
	public static final String PYTHON_WIN_DIR = "python27/";
	public static final String PYTHON_WIN_AMD64_DIR = "python27amd64/";
	public static final String FAST_DOWNWARD_DIR = "fast-downward/";
	public static final String PLANS_FOUND_DIR = FAST_DOWNWARD_DIR + "plans_found/";
	public static final String PDDL_FILES_DIR = FAST_DOWNWARD_DIR + "PDDLFiles/";
	public static final String PDDL_EXT = ".pddl";
	public static final String PDDL_DOMAIN_FILE_PREFIX = PDDL_FILES_DIR + "domain";
	public static final String PDDL_PROBLEM_FILE_PREFIX = PDDL_FILES_DIR + "problem";
	public static final String PLAN_FILE_PREFIX = PLANS_FOUND_DIR + "alignment_";
	public static final String COST_ENTRY_PREFIX = "; cost = ";
	public static final String SEARCH_TIME_ENTRY_PREFIX = "; searchtime = ";
	public static final String TRACE_NAME_PREFIX = "Trace#";
	public static final String COMMAND_ARG_PLACEHOLDER = "+";
	public static final String PLANNER_MANAGER_SCRIPT = "planner_manager.py";
	public static final String FAST_DOWNWARD_SCRIPT = "fast-downward.py";
	
	private static float totalAlignmentCost = 0;
	private static float totalAlignmentTime = 0;

	private static Pattern decimalNumberRegexPattern = Pattern.compile("\\d+(,\\d{3})*(\\.\\d+)*");
	
	static Map<String, List<String>> maxSynchMovesPerPattern = new LinkedHashMap<String,List<String>>();
	static Map<String, List<Trace>> tracesPerPattern = new LinkedHashMap<String, List<Trace>>();
	static List<Integer> indexes = new LinkedList<Integer>();
	

	public static void main(String[] args) throws Exception {
		input = new FileInputStream(propertiesFilePath);
		prop.load(input);
		String input_log= prop.getProperty("input_log");
		String tasks= prop.getProperty("tasks");
		String[] patterns = tasks.split(";");
		int tId = 0;
		int iteration = 1;
		for(String pattern : patterns) {
			List<String> max = new ArrayList<String>();
			maxSynchMovesPerPattern.put(pattern, max);
			List<Trace> traceList = new ArrayList<Trace>();
			tracesPerPattern.put(pattern, traceList);
			List<String> moveSynchs = new ArrayList<String>();
			List<String> moveSynchsCopy = new ArrayList<String>();
			XLog log = XLogReader.openLog(input_log);
			do {
				/**Import della rete di petri in input*/
				PnmlImportUtils ut = new PnmlImportUtils();
				InputStream input = new FileInputStream(new File(pattern));
				Pnml pnml = ut.importPnmlFromStream(input);
		
				Petrinet net = PetrinetFactory.newPetrinet(pnml.getLabel());
				Marking marking = new Marking();								  // only needed for Petrinet initialization
				pnml.convertToNet(net, marking, new GraphLayoutConnection(net));  // initialize Petrinet
		
				Collection<Place> places = net.getPlaces();
				Collection<Transition> transitions = net.getTransitions();
				
				//System.out.println("Number of places: "+places.size());
				//System.out.println("Number of transitions: "+transitions.size());
				
				System.out.println("######################################");
				
				System.out.println("Iteration #"+iteration);
				getTraces(log);
				
				System.out.println("######################################");
				
				
				Constants.setAllTransitionsVector(new Vector<PetrinetTransition>());
				Constants.setAllPlacesVector(new Vector<String>());
				Constants.setPlacesInInitialMarkingVector(new Vector<String>());
				Constants.setPlacesInFinalMarkingVector(new Vector<String>());
				
				//Feed the vector of places with the places imported from the Petri Net.
				//Determine which places compose the initial and final markings.
				for (Place place : places) {
					String placeName = place.getLabel();
					placeName = Utilities.getCorrectFormatting(placeName);
		
					Constants.getAllPlacesVector().addElement(placeName.toLowerCase());
		
					Collection<PetrinetEdge<? extends PetrinetNode, ? extends PetrinetNode>> placeOutEdgesCollection = net.getOutEdges(place);								
					Collection<PetrinetEdge<? extends PetrinetNode, ? extends PetrinetNode>> placeInEdgesCollection = net.getInEdges(place);
		
					if(placeInEdgesCollection.isEmpty())
						Constants.getPlacesInInitialMarkingVector().addElement(placeName);
		
					if(placeOutEdgesCollection.isEmpty())
						Constants.getPlacesInFinalMarkingVector().addElement(placeName);
				}
				
				int generatedTransitionsNum = 0;
				for (Transition transition : transitions) {
					//System.out.println(aTransition.getLabel());
		
					//To get OUTGOING edges from a transition
					Collection<PetrinetEdge<? extends PetrinetNode, ? extends PetrinetNode>> transitionOutEdgesCollection = net.getOutEdges(transition);
		
					//To get INGOING edges to a transition
					Collection<PetrinetEdge<? extends PetrinetNode, ? extends PetrinetNode>> transitionInEdgesCollection = net.getInEdges(transition);
		
		
					Vector<Place> transitionOutPlacesVector = new Vector<Place>();
					Vector<Place> transitionInPlacesVector = new Vector<Place>();
					Iterator<PetrinetEdge<? extends PetrinetNode, ? extends PetrinetNode>> transitionInEdgesIterator = transitionInEdgesCollection.iterator();
					Iterator<PetrinetEdge<? extends PetrinetNode, ? extends PetrinetNode>> transitionOutEdgesIterator = transitionOutEdgesCollection.iterator();
		
		
					while(transitionInEdgesIterator.hasNext()) {
						PetrinetEdge<? extends PetrinetNode, ? extends PetrinetNode> edge = transitionInEdgesIterator.next();									
						transitionInPlacesVector.addElement((Place) edge.getSource());
					}
					
					
					while(transitionOutEdgesIterator.hasNext()) {
						PetrinetEdge<? extends PetrinetNode, ? extends PetrinetNode> edge = transitionOutEdgesIterator.next();
						transitionOutPlacesVector.addElement((Place) edge.getTarget());
		
						//System.out.println(edge.getTarget().getLabel());										
					}
		
					String activityName = transition.getLabel();
		
					if(activityName.isEmpty() || activityName.equalsIgnoreCase("") || activityName.equalsIgnoreCase(" ") || activityName.equalsIgnoreCase("\"")) {
						activityName = new String(INVISIBLE_TRANSITION_PREFIX + generatedTransitionsNum);
						generatedTransitionsNum++;
					}
		
					activityName = Utilities.getCorrectFormatting(activityName);
		
					PetrinetTransition petriNetTransition = new PetrinetTransition(activityName.toLowerCase(), transitionInPlacesVector, transitionOutPlacesVector);
					Constants.getAllTransitionsVector().addElement(petriNetTransition);
				}
				
				//
				// Check if a transition with a specific label appears multiple times in a Petri Net		
				// If so, create a specific alias for the transition 
				//
				for(int ixc=0;ixc<Constants.getAllTransitionsVector().size();ixc++)  {
		
					PetrinetTransition pnt = Constants.getAllTransitionsVector().elementAt(ixc);
					int occurrences = 0;
		
					if(!pnt.isMultiple()) {
		
						for(int j=ixc+1;j<Constants.getAllTransitionsVector().size();j++)  {
		
							PetrinetTransition pnt2 = Constants.getAllTransitionsVector().elementAt(j);
		
							if(pnt2.getName().equalsIgnoreCase(pnt.getName())) {
								if(!pnt.isMultiple()) {
									pnt.setMultiple(true);
									pnt.setAlias(pnt.getName() + "0");
								}
								occurrences ++;
								pnt2.setAlias(pnt.getName() + occurrences);
								pnt2.setMultiple(true);
							}
		
						}
					}
				}
				
				////////////////////////////////////////////////////////////////
				// RESET the alphabet perspective view
				
				Constants.setLogActivitiesRepositoryVector(new Vector<String>());
				
				// RESET the trace perspective view
		
				//Constants.setAllTracesVector(new Vector<Trace>());
				
				// RESET the Petri Nets perspective view
		
				//Constants.setAllPlacesVector(new Vector<String>());
				//Constants.setAllTransitionsVector(new Vector<PetrinetTransition>());
				
				////////////////////////////////////////////////////////////////
				
				int traceId = 0;
		
				// Vector used to record the complete alphabet of activities used in the log
				Vector<String> logAlphabetVector = new Vector<String>();
		
				// Vector used to record the activities of a specific trace of the log
				Vector<String> traceActivitiesVector = new Vector<String>();
		
				//int sumOfTracesLength=0;
		
				for(XTrace trace:log){
		
					traceId++;
		
					//String traceName = XConceptExtension.instance().extractName(trace);
					//System.out.println("Trace Name : " + traceName);
		
					Trace t = new Trace("Trace#" + traceId);
		
					t.setTraceAlphabet(new Vector<String>());
		
					
					traceActivitiesVector = new Vector<String>();
		
					for(XEvent event : trace){
						String activityName = XConceptExtension.instance().extractName(event).toLowerCase();
						activityName = Utilities.getCorrectFormatting(activityName);
		
						traceActivitiesVector.addElement(activityName);
		
						if(!t.getTraceAlphabet().contains(activityName))
							t.getTraceAlphabet().addElement(activityName);
		
						// add activity name to log alphabet (if not already present)
						if(!logAlphabetVector.contains(activityName))
							logAlphabetVector.addElement(activityName);
		
					}
		
					// Update the single trace of the log						
		
					for(int j=0;j<traceActivitiesVector.size();j++) {
						String string = (String) traceActivitiesVector.elementAt(j);
						t.getTraceContentVector().addElement(string);
		
						t.getTraceTextualContent().append(string);
						if(j<traceActivitiesVector.size()-1)
							t.getTraceTextualContent().append(",");
					}
		
					Constants.getAllTracesVector().addElement(t);
					/////////////////////////////////////////////////////////////
		
				}
		
				//Update the GUI component with the loaded LOG
				Constants.setLogActivitiesRepositoryVector(logAlphabetVector);
				
				for(int i=0;i<Constants.getLogActivitiesRepositoryVector().size();i++) {
		
					String string = (String) Constants.getLogActivitiesRepositoryVector().elementAt(i);
		
					Constants.getAllActivitiesVector().addElement(string);
		
					Vector<String> v = new Vector<String>();
					v.addElement(string);
					v.addElement("1");
					v.addElement("1");
					Constants.getActivitiesCostVector().addElement(v);
		
				}
				
				for(int i=0;i<Constants.getAllTransitionsVector().size();i++) {
					PetrinetTransition ith_trans = Constants.getAllTransitionsVector().elementAt(i);
		
					if(!Constants.getLogActivitiesRepositoryVector().contains(ith_trans.getName())) {
		
						Constants.getAllActivitiesVector().addElement(ith_trans.getName());
		
						Vector<String> v = new Vector<String>();
						v.addElement(ith_trans.getName());
		
		
						if(ith_trans.getName().startsWith("generatedinv")) {
							v.addElement("0");
							v.addElement("0");
						}
						else {
							v.addElement("1");
							v.addElement("1");
						}
		
						Constants.getActivitiesCostVector().addElement(v);
		
					}
				}		    	
				
				for(int kind=0;kind<Constants.getAllPlacesVector().size();kind++) {
		
					String place_name = Constants.getAllPlacesVector().elementAt(kind);
		
					Vector<String> v = new Vector<String>();
					v.addElement(place_name);
		
					if(Constants.getPlacesInInitialMarkingVector().contains(place_name)) {
						v.addElement("1");
					}
					else v.addElement("0");
		
					if(Constants.getPlacesInFinalMarkingVector().contains(place_name)) {
						v.addElement("1");
					}
					else v.addElement("0");
		
					Constants.getPetriNetMarkingVector().addElement(v);
		
				}
				
				/**
				 * Computazione dell'allineamento
				 */
				File plansFoundDir = new File(PLANS_FOUND_DIR);
				File pddlFilesDir = new File(PDDL_FILES_DIR);
				Utilities.deleteFolderContents(plansFoundDir);
				Utilities.deleteFolderContents(pddlFilesDir);
				
				for(Trace trace: Constants.getAllTracesVector()){
					StringBuffer sb_domain = Utilities.createPropositionalDomain(trace);
					StringBuffer sb_problem = Utilities.createPropositionalProblem(trace);
			
					Utilities.writeFile(PDDL_FILES_DIR+"domain1.pddl", sb_domain);
					Utilities.writeFile(PDDL_FILES_DIR+"problem1.pddl", sb_problem);	 
				}
				
				
				Planner p = new Planner();
				p.runThePlanner("domain1.pddl","problem1.pddl");
				
				moveSynchs  = extractMoveSynch();
				moveSynchsCopy = new ArrayList<>();
				
				if(moveSynchs.size()!=0) {
					tId++;
					Trace t = new Trace("Trace#" + tId);
					t.setTraceAlphabet(new Vector<String>());
					
					for(String move : moveSynchs) {
						moveSynchsCopy.add(move);
						//if(!t.getTraceAlphabet().contains(move))
						t.getTraceAlphabet().addElement(move);
					}
					
					//aggiungi la traccia in una lista traces of movesynchs
					tracesPerPattern.get(pattern).add(t);
				}
				
				if(moveSynchsCopy.size()> maxSynchMovesPerPattern.get(pattern).size()) {
					maxSynchMovesPerPattern.put(pattern, moveSynchsCopy);
				}
				
				//System.out.println(t.getTraceName()+" "+t.getTraceNumber()+" "+t.getTraceAlphabet().get(0)+" "+t.getTraceAlphabet().get(1));
				//occhio che all'ultima run crasha perchè il trace alignment non trova niente quindi l'ultima traccia sarà vuota
				
				System.out.println("######################################");
				
				log = filterLog(log,moveSynchs);
						
				//getTraces(log);
				//System.out.println("######################################");
		
				indexes = new LinkedList<Integer>();
				iteration++;
			}while(moveSynchsCopy.size()!=0);
		
		//input.close();
		Constants.setLogActivitiesRepositoryVector(new Vector<String>());
		Constants.setAllTracesVector(new Vector<Trace>());
		Constants.setActivitiesCostVector(new Vector<Vector<String>>());
		Constants.setPetriNetMarkingVector(new Vector<Vector<String>>());
		Constants.setAllTransitionsVector(new Vector<PetrinetTransition>());
		Constants.setAllPlacesVector(new Vector<String>());
		Constants.setPlacesInInitialMarkingVector(new Vector<String>());
		Constants.setPlacesInFinalMarkingVector(new Vector<String>());
		Constants.setAllActivitiesVector(new Vector<String>());
		
		
		}
		/**
		 * Creazione del routine based log
		 */
		//---------------------------------------------------------------//
		XFactory factory = XFactoryRegistry.instance().currentDefault();
		XLog newLog = factory.createLog();
		//---------------------------------------------------------------//
		int tid = 1;
		for(String pattern : tracesPerPattern.keySet()) {
			List<Trace> traces = tracesPerPattern.get(pattern);
			for(Trace t : traces) {
				//l'ultima traccia non avrà eventi poichè il trace alignment non trova niente (1° condizione)
				//e devo prendere tutte le tracce che non contengono noise (2° condizione)
				if(t.getTraceAlphabet().size() != 0 && t.getTraceAlphabet().size() == maxSynchMovesPerPattern.get(pattern).size() ) {
					XTrace trace = factory.createTrace();
					XConceptExtension.instance().assignName(trace, "id"+tid);
					for(String ev : t.getTraceAlphabet()) {
						XEvent event = factory.createEvent();
						XConceptExtension.instance().assignName(event, ev);
						Timestamp tm = Utilities.getCurrentTimestamp();
						XLifecycleExtension.instance().assignTransition(event, "complete");
						XTimeExtension.instance().assignTimestamp(event, tm.getTime());
						trace.add(event);
					}
					newLog.add(trace);
					tid++;
				}
				else {
					String str = "";
					for(String ev : t.getTraceAlphabet()) {
						str += ev+" ";
					}
					System.out.println("scarto: "+str);
				}
			}
		}
		
		//---------------------------------------------------------------//
		//creaXES
		File file_for_log = new File("segmented_logs/" + Utilities.getCurrentTimestamp().getTime() + ".xes");
		Utilities.createXESFile(newLog,file_for_log);
		

	}

	private static XLog filterLog(XLog log, List<String> moveSynchs) {
		List<XEvent> toRemove = new ArrayList<XEvent>();
		
		int i = 0;
		for(XTrace trace:log){
			for(XEvent event : trace){
				String activityName = XConceptExtension.instance().extractName(event).toLowerCase();
				String activityNameFormatted = Utilities.getCorrectFormatting(activityName);

				if(moveSynchs.contains(activityNameFormatted) && indexes.contains(i)) {
					System.out.println("Synchronous move -> "+activityName);
					moveSynchs.remove(activityNameFormatted);
					toRemove.add(event);
				}
				i++;
			}
		}
		
		for(XTrace trace:log) {
			for(XEvent event : toRemove)
				trace.remove(event);
		}
		
		return log;
		
	}

	private static void getTraces(XLog log) throws IOException{		

    	System.out.println("UI log:");
		for (XTrace trace : log) {
			trace.forEach(activity -> {
			    System.out.println(activity.getAttributes().get("concept:name").toString());
			});

		}

    }
	
	private static List<String> extractMoveSynch() throws IOException {
		List<String> moveSynchs = new ArrayList<String>();
		
		File alignmentFile = new File(PLANS_FOUND_DIR+"alignment_1");
		
		String traceAlignmentCost = new String();  
		String traceAlignmentTime = new String();  
	
		// parse alignment file
		BufferedReader alignmentFileReader = new BufferedReader(new FileReader(alignmentFile));
		String alignmentFileLine;
		Vector<String> pddlAlignmentMovesVector = new Vector<String>();
		int i = 0;
		
		System.out.println("Trace Alignment:");
		
		while ((alignmentFileLine = alignmentFileReader.readLine()) != null) {
			if(alignmentFileLine.startsWith(COST_ENTRY_PREFIX)) {
				Matcher matcher = decimalNumberRegexPattern.matcher(alignmentFileLine);
				matcher.find();
				traceAlignmentCost = matcher.group();
			}
			else if(alignmentFileLine.startsWith(SEARCH_TIME_ENTRY_PREFIX))  {
				Matcher matcher = decimalNumberRegexPattern.matcher(alignmentFileLine);
				matcher.find();
				traceAlignmentTime = matcher.group();
			}
			else {
				pddlAlignmentMovesVector.addElement(alignmentFileLine);
			}
		}
		alignmentFileReader.close();
		
		
		for(String alignmentMove : pddlAlignmentMovesVector) {

			if(alignmentMove.startsWith("(movesync#")) {

				alignmentMove = alignmentMove.replace("(movesync#", "");
				alignmentMove = alignmentMove.substring(0, alignmentMove.lastIndexOf("#"));

				
				System.out.println(alignmentMove);
				
				moveSynchs.add(alignmentMove);
				indexes.add(i);
				i++;

			}
			else if(alignmentMove.startsWith("(moveinthemodel#")) {

				alignmentMove = alignmentMove.replace("(moveinthemodel#", "");
				alignmentMove = alignmentMove.substring(0,alignmentMove.lastIndexOf(" )"));

				System.out.println(alignmentMove+" [cost " + Utilities.getCostOfActivity(alignmentMove, "move_in_the_model") + "]");
			}
			else if(alignmentMove.startsWith("(moveinthelog#")) {

				alignmentMove = alignmentMove.replace("(moveinthelog#", "");
				alignmentMove = alignmentMove.substring(0,alignmentMove.indexOf("#"));
				i++;



				
				System.out.println(alignmentMove+" [cost " + Utilities.getCostOfActivity(alignmentMove, "move_in_the_log") + "]");
			}
		}
		
		// update total counters
		if(indexes.size()!=0) {
			//for(int c : indexes)
				//System.out.print(c +" ");
			//System.out.println();
			totalAlignmentCost += Float.parseFloat(traceAlignmentCost);
			totalAlignmentTime += Float.parseFloat(traceAlignmentTime);
			System.out.println("ToalAlignmentCost "+totalAlignmentCost+" totalAlignmentTime "+totalAlignmentTime);
		}
		
		return moveSynchs;
	}
	
	

}
