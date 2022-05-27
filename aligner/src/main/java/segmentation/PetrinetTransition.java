package segmentation;

import java.util.Vector;
import org.processmining.models.graphbased.directed.petrinet.elements.Place;

public class PetrinetTransition {

	//Original label of the transition
	private String name;

	private Vector<Place> inputPlacesVector;
	private Vector<Place> outputPlacesVector;

	//Boolean variable that - if holds - indicates that a transition appears multiple times in the Petri Net under analysis
	private boolean isMultiple = false;

	//If a transition appears multiple time in a Petri Net, it requires an alias 
	// For example, if the transition label is "D", its alias will be "D0" (or "D1", "D2", etc...depending on the 
	// occurrences of the transition in the Petri Net)
	private String alias = new String();

	public PetrinetTransition(String transName, Vector<Place> vector_of_input_places, Vector<Place> vector_of_output_places) {		
		name = transName;
		inputPlacesVector = vector_of_input_places;
		outputPlacesVector = vector_of_output_places;
	}

	public String getName() {
		return name;
	}

	public Vector<Place> getInputPlacesVector() {
		return inputPlacesVector;
	}

	public Vector<Place> getOutputPlacesVector() {
		return outputPlacesVector;
	}

	public void setName(String name) {
		this.name = name;
	}

	public void setInputPlacesVector(Vector<Place> inputPlacesVector) {
		this.inputPlacesVector = inputPlacesVector;
	}

	public void setOutputPlacesVector(Vector<Place> outputPlacesVector) {
		this.outputPlacesVector = outputPlacesVector;
	}

	public boolean isMultiple() {
		return isMultiple;
	}

	public String getAlias() {
		return alias;
	}

	public void setMultiple(boolean isMultiple) {
		this.isMultiple = isMultiple;
	}

	public void setAlias(String alias) {
		this.alias = alias;
	}	

}
