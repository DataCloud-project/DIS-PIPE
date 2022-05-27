package segmentation;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

public class Planner {
	
	final String WINDOWS = "windows";
	final String PYTHON_WIN_DIR = "python27/";
	final String PYTHON_WIN_AMD64_DIR = "python27amd64/";
	final String FAST_DOWNWARD_DIR = "fast-downward/";
	final String PLANS_FOUND_DIR = FAST_DOWNWARD_DIR + "plans_found/";
	final String PDDL_FILES_DIR = FAST_DOWNWARD_DIR + "PDDLfiles/";
	final String PDDL_EXT = ".pddl";
	final String PDDL_DOMAIN_FILE_PREFIX = PDDL_FILES_DIR + "domain";
	final String PDDL_PROBLEM_FILE_PREFIX = PDDL_FILES_DIR + "problem";
	final String SEARCH_TIME_ENTRY_PREFIX = "; searchtime = ";
	final String COMMAND_ARG_PLACEHOLDER = "+";
	final String PLANNER_MANAGER_SCRIPT = "planner_manager.py";
	final String FAST_DOWNWARD_SCRIPT = "fast-downward.py";

	public static boolean is64bitsOS() {
	    String osArch = System.getProperty("os.arch");
	    String winArch = System.getenv("PROCESSOR_ARCHITECTURE");
	    String wow64Arch = System.getenv("PROCESSOR_ARCHITEW6432");
	    
	    return osArch != null && osArch.endsWith("64") || winArch != null && winArch.endsWith("64") || wow64Arch != null && wow64Arch.endsWith("64");
	}
	
	void runThePlanner(String domainFile, String problemFile){
	    String[] commandArgs;
	    String domain=PDDL_FILES_DIR+domainFile, problem=PDDL_FILES_DIR+problemFile;
	    try{
	          //compute domain and problem
	          
	          //System.out.println(domain+" "+problem);
	          commandArgs = buildFastDownardCommandArgs(domain,problem);
	          //System.out.println(java.util.Arrays.toString(commandArgs));
	          
	          // execute external planner script and wait for results
	          ProcessBuilder processBuilder = new ProcessBuilder(commandArgs);
	          Process plannerManagerProcess = processBuilder.start();
	          
	          // read std out & err in separated thread
	          StreamGobbler errorGobbler = new StreamGobbler(plannerManagerProcess.getErrorStream(), "ERROR");
	          StreamGobbler outputGobbler = new StreamGobbler(plannerManagerProcess.getInputStream(), "OUTPUT");
	          errorGobbler.start();
	          outputGobbler.start();

	          // wait for the process to return to read the generated outputs
	          plannerManagerProcess.waitFor();
	    }
	    catch(Exception e){
	          e.printStackTrace();
	    }
	}

	String[] buildFastDownardCommandArgs(String domain, String problem) throws IOException {
	    ArrayList<String> commandComponents = new ArrayList();

	    // determine which python interpreter must be used
	    String pythonInterpreter = "python";
	    
	    String osName = System.getProperty("os.name").toLowerCase();
	    if (osName.contains("windows")) {
	      if (is64bitsOS()) {
	        pythonInterpreter = PYTHON_WIN_AMD64_DIR + pythonInterpreter;
	      } else {
	        pythonInterpreter = PYTHON_WIN_DIR + pythonInterpreter;
	      }
	    }

	    /* begin of command args for planner manager */

	    commandComponents.add(pythonInterpreter);

	    File plannerManagerScript = new File(PLANNER_MANAGER_SCRIPT);
	    commandComponents.add(plannerManagerScript.getCanonicalPath());


	    /* begin of command args for Fast-Downward */

	    commandComponents.add(pythonInterpreter);

	    File fdScript = new File(FAST_DOWNWARD_DIR + FAST_DOWNWARD_SCRIPT);
	    commandComponents.add(fdScript.getCanonicalPath());

	    // Fast-Downward is assumed to be built in advance both for 32 and 64 bits OS (being them Windows or Unix-like).
	    commandComponents.add("--build");
	    if (is64bitsOS())
	      commandComponents.add("release64");
	    else
	      commandComponents.add("release32");

	    commandComponents.add("--plan-file");
	    commandComponents.add("plan-of-actions");  // output file

	    commandComponents.add(domain);  // domain file
	    commandComponents.add(problem);  // problem file

	    commandComponents.add("--heuristic");
	    commandComponents.add("hcea=cea()");
	    commandComponents.add("--search");
	    commandComponents.add("astar(blind())");
	 

	    String[] commandArguments = commandComponents.toArray(new String[0]);
	    return commandArguments;
	}
}
