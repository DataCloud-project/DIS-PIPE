

def extract_transitions(file_path):
    transitions = []

    # Parse the XML file
    tree = ET.parse(file_path)
    root = tree.getroot()

    # Find all transition elements
    transition_elements = root.findall(".//transition")

    # Iterate over the transition elements
    for transition in transition_elements:
        # Get the transition ID
        transition_id = transition.attrib["id"]

        # Find the text tag within the transition element
        text_element = transition.find(".//text")

        if text_element is not None:
            # Get the text value
            text = text_element.text
        else:
            text = None

        # Add the transition ID and text to the result array
        transitions.append({"id": transition_id, "text": text})

    return transitions


def rephrase_text(file_path, transition_id, new_text, file_path_end):
    # Parse the XML file
    tree = ET.parse(file_path)
    root = tree.getroot()

    # Find the transition element with the given ID
    transition_element = root.find(".//transition[@id='" + transition_id + "']")

    if transition_element is not None:
        # Find the text tag within the transition element
        text_element = transition_element.find(".//text")

        if text_element is not None:
            # Update the text value
            text_element.text = new_text
        else:
            # Create a new text element and set its value
            text_element = ET.SubElement(transition_element, "text")
            text_element.text = new_text
    else:
        # Transition not found
        return False

    # Save the modified XML to file
    tree.write(file_path_end)

    return True

def retrieve_id(text, array):
    for item in array:
        if item['text'] == text:
            return item['id']
    return None


result = extract_transitions("petri_final.pnml")
file_path = "petri_final.pnml"
file_path_end = "petri_final_remap.pnml"
transition_id = retrieve_id("examine_thoroughly-End",result)
new_text = "DarkPoloGang777"

success = rephrase_text(file_path, transition_id, new_text,file_path_end)
if success:
    print("Text updated successfully.")
else:
    print("Transition ID not found.")