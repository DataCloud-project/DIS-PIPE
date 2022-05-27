package segmentation;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.sql.Timestamp;
import java.util.Arrays;
import java.util.Vector;

import org.deckfour.xes.extension.std.XConceptExtension;
import org.deckfour.xes.extension.std.XLifecycleExtension;
import org.deckfour.xes.extension.std.XTimeExtension;
import org.deckfour.xes.factory.XFactory;
import org.deckfour.xes.factory.XFactoryRegistry;
import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;
import org.deckfour.xes.out.XesXmlSerializer;
import org.processmining.models.graphbased.directed.petrinet.elements.Place;

public class Utilities {

	public static StringBuffer createPropositionalDomain(Trace trace) {

		StringBuffer PDDL_domain_buffer = new StringBuffer();

		PDDL_domain_buffer.append("(define (domain Mining)\n");
		PDDL_domain_buffer.append("(:requirements :typing :equality)\n");
		PDDL_domain_buffer.append("(:types place event)\n\n");

		PDDL_domain_buffer.append("(:predicates\n");	
		PDDL_domain_buffer.append("(token ?p - place)\n");			
		PDDL_domain_buffer.append("(tracePointer ?e - event)\n");
		PDDL_domain_buffer.append("(allowed)\n");		
		PDDL_domain_buffer.append(")\n\n");			

		PDDL_domain_buffer.append("(:functions\n");	
		PDDL_domain_buffer.append("(total-cost)\n");			
		PDDL_domain_buffer.append(")\n\n");		

		for(int i=0;i<Constants.getAllTransitionsVector().size();i++) {

			PetrinetTransition ith_transition = Constants.getAllTransitionsVector().elementAt(i);

			//Move Sync
			for(int k=0;k<trace.getTraceContentVector().size();k++) {

				String elem_of_the_trace = trace.getTraceContentVector().elementAt(k);
				int ev_curr_index = k + 1;
				int ev_next_index = k + 2;
				String curr_event = "ev" + ev_curr_index;

				if(elem_of_the_trace.equalsIgnoreCase(ith_transition.getName())) {
					PDDL_domain_buffer.append("(:action moveSync" + "#" + ith_transition.getName() + "#" + curr_event + "\n");
					PDDL_domain_buffer.append(":precondition (and");
					for(int lin=0;lin<ith_transition.getInputPlacesVector().size();lin++) {
						Place p = ith_transition.getInputPlacesVector().elementAt(lin);
						PDDL_domain_buffer.append(" (token " + Utilities.getCorrectFormatting(p.getLabel()) + ")");
					}
					PDDL_domain_buffer.append(" (tracePointer "+ curr_event + ")");
					PDDL_domain_buffer.append(")\n");

					PDDL_domain_buffer.append(":effect (and (allowed)");
					for(int lin=0;lin<ith_transition.getInputPlacesVector().size();lin++) {
						Place p = ith_transition.getInputPlacesVector().elementAt(lin);
						PDDL_domain_buffer.append(" (not (token " + Utilities.getCorrectFormatting(p.getLabel()) + "))");
					}
					for(int lin=0;lin<ith_transition.getOutputPlacesVector().size();lin++) {
						Place p = ith_transition.getOutputPlacesVector().elementAt(lin);
						PDDL_domain_buffer.append(" (token " + Utilities.getCorrectFormatting(p.getLabel()) + ")");
					}

					String next_event;
					if(ev_curr_index==trace.getTraceContentVector().size())
						next_event = "evEND";
					else
						next_event = "ev" + ev_next_index;

					PDDL_domain_buffer.append(" (not (tracePointer "+ curr_event + ")) (tracePointer "+ next_event + ")");
					PDDL_domain_buffer.append(")\n");
					PDDL_domain_buffer.append(")\n\n");
				}

			}


			///////////////////////////////////////////////////////////////////////////
			//Move in the Model
			PDDL_domain_buffer.append("(:action moveInTheModel" + "#" + ith_transition.getName() + "\n");
			PDDL_domain_buffer.append(":precondition");

			if(ith_transition.getInputPlacesVector().size()>1)
				PDDL_domain_buffer.append(" (and");

			for(int lin=0;lin<ith_transition.getInputPlacesVector().size();lin++) {
				Place p = ith_transition.getInputPlacesVector().elementAt(lin);
				PDDL_domain_buffer.append(" (token " + Utilities.getCorrectFormatting(p.getLabel()) + ")");
			}

			if(ith_transition.getInputPlacesVector().size()>1)
				PDDL_domain_buffer.append(")\n");
			else
				PDDL_domain_buffer.append("\n");

			PDDL_domain_buffer.append(":effect (and (not (allowed))");

			for(int lin=0;lin<ith_transition.getInputPlacesVector().size();lin++) {
				Place p = ith_transition.getInputPlacesVector().elementAt(lin);
				PDDL_domain_buffer.append(" (not (token " + Utilities.getCorrectFormatting(p.getLabel()) + "))");
			}
			for(int lin=0;lin<ith_transition.getOutputPlacesVector().size();lin++) {
				Place p = ith_transition.getOutputPlacesVector().elementAt(lin);
				PDDL_domain_buffer.append(" (token " + Utilities.getCorrectFormatting(p.getLabel()) + ")");
			}				

			
			PDDL_domain_buffer.append(" (increase (total-cost) ");	

				for(int yu=0;yu<Constants.getActivitiesCostVector().size();yu++) {
					Vector<String> specificTraceCostVector = Constants.getActivitiesCostVector().elementAt(yu);					
					if(specificTraceCostVector.elementAt(0).equalsIgnoreCase(ith_transition.getName())) {
						PDDL_domain_buffer.append(specificTraceCostVector.elementAt(1) + ")\n");
						break;
					}
				}
			

			PDDL_domain_buffer.append(")\n");
			PDDL_domain_buffer.append(")\n\n");
		}

		//Move in the Log

		for(int itr=0;itr<trace.getTraceContentVector().size();itr++) {

			String activity_related_to_the_event = trace.getTraceContentVector().elementAt(itr);

			int index_current_trace = itr + 1;
			int index_next_trace = itr + 2;

			String current_event = "ev" + index_current_trace;

			String next_event;
			if(index_current_trace==trace.getTraceContentVector().size())
				next_event = "evEND";
			else
				next_event = "ev" + index_next_trace;

			PDDL_domain_buffer.append("(:action moveInTheLog#" + activity_related_to_the_event + "#" + current_event + "-" + next_event + "\n");
			PDDL_domain_buffer.append(":precondition (and (tracePointer " + current_event  + ") (allowed))\n");
			PDDL_domain_buffer.append(":effect (and (not (tracePointer " + current_event  + ")) (tracePointer " + next_event  + ")");

				PDDL_domain_buffer.append(" (increase (total-cost) ");	

				for(int yu=0;yu<Constants.getActivitiesCostVector().size();yu++) {
					Vector<String> specificTraceCostVector = Constants.getActivitiesCostVector().elementAt(yu);					
					if(specificTraceCostVector.elementAt(0).equalsIgnoreCase(activity_related_to_the_event)) {
						PDDL_domain_buffer.append(specificTraceCostVector.elementAt(2) + ")\n");
						break;
					}
				}
			
			PDDL_domain_buffer.append(")");
			PDDL_domain_buffer.append(")\n\n");	
		}

		PDDL_domain_buffer.append(")");
		return PDDL_domain_buffer;
	}

	public static StringBuffer createPropositionalProblem(Trace trace) {

		StringBuffer PDDL_objects_buffer = new StringBuffer();	
		StringBuffer PDDL_init_buffer = new StringBuffer();
		StringBuffer PDDL_cost_buffer = new StringBuffer();
		StringBuffer PDDL_goal_buffer = new StringBuffer();
		StringBuffer PDDL_problem_buffer = new StringBuffer();

		PDDL_objects_buffer.append("(define (problem Align) (:domain Mining)\n");
		PDDL_objects_buffer.append("(:objects\n");	

		PDDL_init_buffer = new StringBuffer("(:init\n");
		PDDL_init_buffer.append("(tracePointer ev1)\n");
		PDDL_init_buffer.append("(allowed)\n");

		PDDL_goal_buffer.append("(:goal\n");
		PDDL_goal_buffer.append("(and\n");

		for(int kj=0;kj<Constants.getPetriNetMarkingVector().size();kj++) {
			Vector<String> v = Constants.getPetriNetMarkingVector().elementAt(kj);

			if(v.elementAt(1).equalsIgnoreCase("1")) 
				PDDL_init_buffer.append("(token " + v.elementAt(0) + ")\n");

			if(v.elementAt(2).equalsIgnoreCase("1")) 
				PDDL_goal_buffer.append("(token " + v.elementAt(0) + ")\n");
			else if(v.elementAt(2).equalsIgnoreCase("0")) 
				PDDL_goal_buffer.append("(not (token " + v.elementAt(0) + "))\n");
		}

		for(int hindex=0;hindex<Constants.getAllPlacesVector().size();hindex++) {
			String placeName = Constants.getAllPlacesVector().elementAt(hindex);
			PDDL_objects_buffer.append(placeName + " - place\n");
		}


		for(int sd=0;sd<trace.getTraceContentVector().size();sd++) {	
			int sd_ix = sd + 1;
			//int sd_ix_next = sd_ix + 1;
			PDDL_objects_buffer.append("ev" + sd_ix + " - event\n");		

			if(sd_ix == trace.getTraceContentVector().size()) {
				PDDL_objects_buffer.append("evEND - event\n");					
			}

		}	
		PDDL_objects_buffer.append(")\n");		



			PDDL_cost_buffer.append("(= (total-cost) 0)\n");
			PDDL_init_buffer.append(PDDL_cost_buffer);
		

		PDDL_init_buffer.append(")\n");

		PDDL_goal_buffer.append("(tracePointer evEND)\n");
		PDDL_goal_buffer.append("))\n");

		
		PDDL_goal_buffer.append("(:metric minimize (total-cost))\n");	

		PDDL_problem_buffer.append(PDDL_objects_buffer);
		PDDL_problem_buffer.append(PDDL_init_buffer);
		PDDL_problem_buffer.append(PDDL_goal_buffer);	
		PDDL_problem_buffer.append(")");	

		return PDDL_problem_buffer;
	}

	public static File writeFile(String nomeFile, StringBuffer buffer) {

		File file = null;
		FileWriter fw = null;

		try {
			file = new File(nomeFile);
			file.setExecutable(true);

			fw = new FileWriter(file);
			fw.write(buffer.toString());
			fw.close();

			//fw.flush();
			//fw.close();
			
			return file;
		}
		catch(IOException e) {
			e.printStackTrace();
		}
		
		return null;
	}
	
	/**
	 * Check whether the OS is 64 bits.
	 * 
	 * @return true if OS is 64 bits.
	 */
	public static boolean is64bitsOS() {
		String osArch = System.getProperty("os.arch");
		String winArch = System.getenv("PROCESSOR_ARCHITECTURE");
		String wow64Arch = System.getenv("PROCESSOR_ARCHITEW6432");
		
		return osArch != null && osArch.endsWith("64") || winArch != null && winArch.endsWith("64") || wow64Arch != null && wow64Arch.endsWith("64");
	}

	public static String getCorrectFormatting(String string)  {

		if(string.contains(" "))
			string = string.replaceAll(" ", "");

		if(string.contains("/"))
			string = string.replaceAll("\\/", "");

		if(string.contains("("))
			string = string.replaceAll("\\(", "");

		if(string.contains(")"))
			string = string.replaceAll("\\)", "");

		if(string.contains("<"))
			string = string.replaceAll("\\<", "");

		if(string.contains(">"))
			string = string.replaceAll("\\>", "");

		if(string.contains("."))
			string = string.replaceAll("\\.", "");

		if(string.contains(","))
			string = string.replaceAll("\\,", "_");

		if(string.contains("+"))
			string = string.replaceAll("\\+", "_");

		if(string.contains("-"))
			string = string.replaceAll("\\-", "_");

		return string;
	}

	public static boolean isInteger(String s) {
		try { 
			Integer.parseInt(s); 
		} catch(NumberFormatException e) { 
			return false; 
		} catch(NullPointerException e) {
			return false;
		}
		// only got here if we didn't return false
		return true;
	}

	/**
	 * Method that creates a XES file starting from a XLog passed as input.
	 */
	public static File createXESFile(XLog eventLog, File outFile) throws IOException {
		OutputStream outStream = new FileOutputStream(outFile);
		new XesXmlSerializer().serialize(eventLog,outStream);
		return outFile;
	}

	/**
	 * Method that returns the current Timestamp.
	 */
	public static Timestamp getCurrentTimestamp() {
		java.util.Date date = new java.util.Date();
		return new Timestamp(date.getTime());
	}

	public static void createXLog() throws IOException {

		//---------------------------------------------------------------//
		XFactory factory = XFactoryRegistry.instance().currentDefault();
		XLog log = factory.createLog();
		//---------------------------------------------------------------//

		File folder = new File("fast-downward/src/plans_found/hmax/62/30NOISE");
		File[] listOfFiles = folder.listFiles();

		Arrays.sort(listOfFiles);
		int traces_noised_int = 0;
		int total_noise_steps = 0;

		for (int i = 0; i < listOfFiles.length; i++) {

			File file = listOfFiles[i];

			//---------------------------------------------------------------//
			XTrace trace = factory.createTrace();
			XConceptExtension.instance().assignName(trace, "id"+i);
			//---------------------------------------------------------------//		

			boolean trace_with_noise = false;

			if (file.isFile()) {

				System.out.println("FILE NAME : " + file.getName());
				BufferedReader br = new BufferedReader(new FileReader(file));           

				try {
					String line = br.readLine();

					trace_with_noise = false;

					int noise_steps = 0;

					while (line != null) {

						boolean event_found = false;

						System.out.println(line);

						if(line.contains("(movesync-")) {
							line = line.replace("(movesync-", "");
							int index = line.lastIndexOf("-");
							line = line.substring(0,index);
							event_found = true;
						}

						if(line.contains("(moveinthemodel-")) {
							line = line.replace("(moveinthemodel-", "");
							event_found = true;


							if(!line.contains("generatedinv")) {
								trace_with_noise = true;
								noise_steps++;
							}

						}

						if(line.contains("(moveinthelog-")) {
							trace_with_noise = true;
							noise_steps++;
						}

						if(event_found) {
							XEvent event = factory.createEvent();
							XConceptExtension.instance().assignName(event, line);

							Timestamp tm = Utilities.getCurrentTimestamp();

							XLifecycleExtension.instance().assignTransition(event, "complete");
							XTimeExtension.instance().assignTimestamp(event, tm.getTime());

							trace.add(event);
						}

						line = br.readLine();


					}
					System.out.println("NOISE STEPS : " + noise_steps);
					total_noise_steps += noise_steps;
					System.out.println("TOTAL NOISE STEPS : " + total_noise_steps);

				} finally {
					br.close();
				}

				log.add(trace);

				if(trace_with_noise)
					traces_noised_int++;

			} 

			System.out.println("Number of traces with noise : " + traces_noised_int);

		}


		File file_for_log = new File("aligned_logs/" + Utilities.getCurrentTimestamp().getTime() + ".xes");
		try {
			Utilities.createXESFile(log,file_for_log);
		} 
		catch (IOException e) {
			e.printStackTrace();
		}	

	}

	public static boolean isUpperCase(String str){

		for(int i=0; i<str.length(); i++){
			char c = str.charAt(i);

			if(Character.isUpperCase(c))
				return true;
		}
		return false;

	}

	public static void deleteFolderContents(File folder) {
		File[] files = folder.listFiles();
		if(files!=null) { 
			for(File f: files) {
				if(f.isDirectory()) {
					deleteFolderContents(f);
				} else {
					f.delete();
				}
			}
		}
	}

	public static String getCostOfActivity(String activityName, String type_of_cost) {

		for(int index=0;index<Constants.getActivitiesCostVector().size();index++) {							
			Vector<String> v = Constants.getActivitiesCostVector().elementAt(index);
			if(v.elementAt(0).equalsIgnoreCase(activityName)) {
				if(type_of_cost == "move_in_the_model")
					return(v.elementAt(1));
				else if(type_of_cost == "move_in_the_log")
					return(v.elementAt(2));
			}
		}
		return "";
	}

}
