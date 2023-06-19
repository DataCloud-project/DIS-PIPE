package segmentation;

import java.util.Vector;

public class Trace {

	private String name;
	private Vector<String> traceAlphabet;
	private Vector<String> traceContentVector;
	private int traceLength;
	private StringBuffer traceTextualContent;

	public Trace(String traceName) {
		name = traceName;
		traceContentVector = new Vector<String>();
		traceTextualContent = new StringBuffer();
	}

	public String getTraceName() {
		return name;
	}

	public void setTraceName(String traceName) {
		name = traceName;
	}

	public Vector<String> getTraceContentVector() {
		return traceContentVector;
	}

	public void setTraceContentVector(Vector<String> traceContent) {
		traceContentVector = traceContent;
	}

	public Vector<String> getTraceAlphabet() {
		return traceAlphabet;
	}

	public void setTraceAlphabet(Vector<String> traceAlphabet) {
		this.traceAlphabet = traceAlphabet;
	}

	public String getTraceNumber() {
		String[] split = this.getTraceName().split("#");
		return split[1];		
	}

	public StringBuffer getTraceTextualContent() {
		return traceTextualContent;
	}

	public void setTraceTextualContent(StringBuffer traceTextualContent) {
		this.traceTextualContent = traceTextualContent;
	}

	public int getTraceLength() {
		return traceLength;
	}

	public void setTraceLength(int traceLenght) {
		this.traceLength = traceLenght;
	}

}