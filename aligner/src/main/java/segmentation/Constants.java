package segmentation;

import java.util.Hashtable;
import java.util.Vector;
import javax.swing.JFrame;

public class Constants {

	public static final String TIME_UNIT = " ms.";
	
	private static JFrame desktop;

	/**
	 * Vector that records the alphabet of activities that appear in the log traces.
	 */
	private static Vector<String> log_activities_repository_vector = new Vector<String>();	

	/**
	 * Vector that records all the traces (represented as java objects "Trace") of the log.	
	 */
	private static Vector<Trace> all_traces_vector = new Vector<Trace>();	

	/**
	 * Hashtable that records the name of a reference trace and its corresponding content.
	 */
	private static Hashtable<String, String> all_traces_hashtable = new Hashtable<String, String>();	

	/**
	 * Vector that records all the transitions (represented as java objects "PetriNetTransition") of the log.	
	 */
	private static Vector<PetrinetTransition> all_transitions_vector = new Vector<PetrinetTransition>();
	
	/**
	 * Vector that records all the places (represented as java Strings) of the log.	
	 */
	private static Vector<String> all_places_vector = new Vector<String>();

	/**
	 * Vectors that record the places having one token in the initial marking.	
	 */
	private static Vector<String> places_in_initial_marking_vector = new Vector<String>();
	
	/**
	 * Vectors that record the places having one token in the final marking.	
	 */
	private static Vector<String> places_in_final_marking_vector = new Vector<String>();	

	/**
	 * Vector that records the cost of moving activities only in the model or in the log.
	 * It is a Vector of Vectors, where each Vector is built in the following way:
	 * - the first element is the name of the activity, 
	 * - the second element is the cost of moving an activity in the model
	 * - the third element is the cost of moving an activity in the log
	 */
	private static Vector<Vector<String>> activities_cost_vector = new Vector<Vector<String>>();	

	/**
	 * Vector that records the initial/final marking of any place included in the Petri Net.
	 * It is a Vector of Vectors, where each Vector is built in the following way:
	 * - the first element is the name of the place,
	 * - the second element is the number of tokens in the initial marking of the place
	 * - the third element is the number of tokens in the final marking of the place		
	 */
	private static Vector<Vector<String>> petriNet_marking_vector = new Vector<Vector<String>>();	
	
	/**
	 * Vector that records the complete alphabet of activities that appear in the log and in the Petri Net.
	 * Notice that the alphabet of activities may include activities that are included in some trace,
	 * but never used in the Petri Net, and vice-versa.		
	 */
	private static Vector<String> all_activities_vector = new Vector<String>();

	private static int minimum_length_of_a_trace = 0;
	private static int maximum_length_of_a_trace = 0;
	private static boolean discard_duplicated_traces = false;
	private static String eventLog_file_name = new String("Created from scratch");
	private static String petriNet_file_name = new String();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////// GETTERS AND SETTERS ///////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	public static JFrame getDesktop() {
		return desktop;
	}
	public static void setDesktop(JFrame desk) {
		desktop = desk;
	}
	
	public static Vector<String> getLogActivitiesRepositoryVector() {
		return log_activities_repository_vector;
	}
	public static void setLogActivitiesRepositoryVector(Vector<String> v) {
		log_activities_repository_vector = v;
	}
	public static Vector<Trace> getAllTracesVector() {
		return all_traces_vector;
	}
	public static void setAllTracesVector(Vector<Trace> all_traces_vector) {
		Constants.all_traces_vector = all_traces_vector;
	}
	public static Vector<Vector<String>> getActivitiesCostVector() {
		return activities_cost_vector;
	}
	public static void setActivitiesCostVector(Vector<Vector<String>> cost_vector) {
		Constants.activities_cost_vector = cost_vector;
	}
	public static Vector<Vector<String>> getPetriNetMarkingVector() {
		return petriNet_marking_vector;
	}
	public static void setPetriNetMarkingVector(Vector<Vector<String>> petriNet_marking_vector) {
		Constants.petriNet_marking_vector = petriNet_marking_vector;
	}
	public static Vector<PetrinetTransition> getAllTransitionsVector() {
		return all_transitions_vector;
	}
	public static void setAllTransitionsVector(Vector<PetrinetTransition> all_transitions_vector) {
		Constants.all_transitions_vector = all_transitions_vector;
	}
	public static Vector<String> getAllPlacesVector() {
		return all_places_vector;
	}
	public static void setAllPlacesVector(Vector<String> all_places_vector) {
		Constants.all_places_vector = all_places_vector;
	}
	public static Vector<String> getPlacesInInitialMarkingVector() {
		return places_in_initial_marking_vector;
	}
	public static void setPlacesInInitialMarkingVector(Vector<String> places_in_initial_marking_vector) {
		Constants.places_in_initial_marking_vector = places_in_initial_marking_vector;
	}
	public static Vector<String> getPlacesInFinalMarkingVector() {
		return places_in_final_marking_vector;
	}
	public static void setPlacesInFinalMarkingVector(Vector<String> places_in_final_marking_vector) {
		Constants.places_in_final_marking_vector = places_in_final_marking_vector;
	}
	public static Vector<String> getAllActivitiesVector() {
		return all_activities_vector;
	}
	public static void setAllActivitiesVector(Vector<String> all_activities_repository_vector) {
		Constants.all_activities_vector = all_activities_repository_vector;
	}
	public static Hashtable<String, String> getAllTracesHashtable() {
		return all_traces_hashtable;
	}
	public static void setAllTracesHashtable(Hashtable<String, String> all_traces_hashtable) {
		Constants.all_traces_hashtable = all_traces_hashtable;
	}
	public static int getMinimumLengthOfATrace() {
		return minimum_length_of_a_trace;
	}
	public static int getMaximumLengthOfATrace() {
		return maximum_length_of_a_trace;
	}
	public static void setMinimumLengthOfATrace(int minimum_length_of_a_trace) {
		Constants.minimum_length_of_a_trace = minimum_length_of_a_trace;
	}
	public static void setMaximumLengthOfATrace(int maximum_length_of_a_trace) {
		Constants.maximum_length_of_a_trace = maximum_length_of_a_trace;
	}
	public static boolean isDiscardDuplicatedTraces() {
		return discard_duplicated_traces;
	}
	public static void setDiscardDuplicatedTraces(boolean discard_duplicated_traces) {
		Constants.discard_duplicated_traces = discard_duplicated_traces;
	}
	public static String getEventLogFileName() {
		return eventLog_file_name;
	}
	public static void setEventLogFileName(String eventLog_file_name) {
		Constants.eventLog_file_name = eventLog_file_name;
	}
	public static String getPetriNetFileName() {
		return petriNet_file_name;
	}
	public static void setPetriNetFileName(String petriNet_file_name) {
		Constants.petriNet_file_name = petriNet_file_name;
	}	

}
