package segmentation;

import java.io.InputStream;

import org.processmining.plugins.pnml.Pnml;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

public class PnmlImportUtils {
	public Pnml importPnmlFromStream(InputStream input)
			throws Exception {
		/*
		 * Get an XML pull parser.
		 */
		XmlPullParserFactory factory = XmlPullParserFactory.newInstance();
		factory.setNamespaceAware(true);
		XmlPullParser xpp = factory.newPullParser();
		/*
		 * Initialize the parser on the provided input.
		 */
		xpp.setInput(input, null);
		/*
		 * Get the first event type.
		 */
		int eventType = xpp.getEventType();
		/*
		 * Create a fresh PNML object.
		 */
		Pnml pnml = new Pnml();

		/*
		 * Skip whatever we find until we've found a start tag.
		 */
		while (eventType != XmlPullParser.START_TAG) {
			eventType = xpp.next();
		}
		/*
		 * Check whether start tag corresponds to PNML start tag.
		 */
		if (xpp.getName().equals(Pnml.TAG)) {
			/*
			 * Yes it does. Import the PNML element.
			 */
			pnml.importElement(xpp, pnml);
		} else {
			/*
			 * No it does not. Return null to signal failure.
			 */
			pnml.log(Pnml.TAG, xpp.getLineNumber(), "Expected pnml");
		}
		return pnml;
	}


}
