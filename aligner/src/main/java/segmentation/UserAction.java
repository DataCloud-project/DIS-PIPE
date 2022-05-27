package segmentation;

public class UserAction {

	private String timestamp;
	private String user;
	private String category;
	private String application;
	private String event_type;
	private String event_src_path;
	private String event_dest_path;
	private String clipboard_content;
	private String workbook;
	private String current_worksheet;
	private String worksheets;
	private String sheets;
	private String cell_content;
	private String cell_range;
	private String window_size;
	private String slides;
	private String effect;
	private String id;
	private String title;
	private String description;
	private String browser_url;
	private String eventQual;
	private String tab_moved_from_index;
	private String tab_moved_to_index;
	private String newZoomFactor;
	private String oldZoomFactor;
	private String tab_pinned;
	private String tab_audible;
	private String tab_muted;
	private String window_ingognito;
	private String file_size;
	private String tag_category;
	private String tag_type;
	private String tag_name;
	private String tag_title;
	private String tag_value;
	private String tag_checked;
	private String tag_html;
	private String tag_href;
	private String tag_innerText;
	private String tag_option;
	
	UserAction(){
		super();
	}
	UserAction(String row) {
		// TODO Auto-generated constructor stub
		String[] data = row.split(",",41);
		setTimestamp(data[0]);
		setUser(data[1]);
		setCategory(data[2]);
		setApplication(data[3]);
		setEvent_type(data[4]);
		setEvent_src_path(data[5]);
		setEvent_dest_path(data[6]);
		setClipboard_content(data[7]);
		setWorkbook(data[8]);
		setCurrent_worksheet(data[9]);
		setWorksheets(data[10]);
		setSheets(data[11]);
		setCell_content(data[12]);
		setCell_range(data[13]);
		setWindow_size(data[14]);
		setSlides(data[15]);
		setEffect(data[16]);
		setId(data[17]);
		setTitle(data[18]);
		setDescription(data[19]);
		setBrowser_url(data[20]);
		setEventQual(data[21]);
		setTab_moved_from_index(data[22]);
		setTab_moved_to_index(data[23]);
		setNewZoomFactor(data[24]);
		setOldZoomFactor(data[25]);
		setTab_pinned(data[26]);
		setTab_audible(data[27]);
		setTab_muted(data[28]);
		setWindow_ingognito(data[29]);
		setFile_size(data[30]);
		setTag_category(data[31]);
		setTag_type(data[32]);
		setTag_name(data[33]);
		setTag_title(data[34]);
		setTag_value(data[35]);
		setTag_checked(data[36]);
		setTag_html(data[37]);
		setTag_href(data[38]);
		setTag_innerText(data[39]);
		setTag_option(data[40]);
	}


	public String getTimestamp() {
		return timestamp;
	}


	public void setTimestamp(String timestamp) {
		this.timestamp = timestamp;
	}


	public String getUser() {
		return user;
	}


	public void setUser(String user) {
		this.user = user;
	}


	public String getCategory() {
		return category;
	}


	public void setCategory(String category) {
		this.category = category;
	}


	public String getApplication() {
		return application;
	}


	public void setApplication(String application) {
		this.application = application;
	}


	public String getEvent_type() {
		return event_type;
	}


	public void setEvent_type(String event_type) {
		this.event_type = event_type;
	}


	public String getEvent_src_path() {
		return event_src_path;
	}


	public void setEvent_src_path(String event_src_path) {
		this.event_src_path = event_src_path;
	}


	public String getEvent_dest_path() {
		return event_dest_path;
	}


	public void setEvent_dest_path(String event_dest_path) {
		this.event_dest_path = event_dest_path;
	}


	public String getClipboard_content() {
		return clipboard_content;
	}


	public void setClipboard_content(String clipboard_content) {
		this.clipboard_content = clipboard_content;
	}


	public String getWorkbook() {
		return workbook;
	}


	public void setWorkbook(String workbook) {
		this.workbook = workbook;
	}


	public String getCurrent_worksheet() {
		return current_worksheet;
	}


	public void setCurrent_worksheet(String current_worksheet) {
		this.current_worksheet = current_worksheet;
	}


	public String getWorksheets() {
		return worksheets;
	}


	public void setWorksheets(String worksheets) {
		this.worksheets = worksheets;
	}


	public String getSheets() {
		return sheets;
	}


	public void setSheets(String sheets) {
		this.sheets = sheets;
	}


	public String getCell_content() {
		return cell_content;
	}


	public void setCell_content(String cell_content) {
		this.cell_content = cell_content;
	}


	public String getCell_range() {
		return cell_range;
	}


	public void setCell_range(String cell_range) {
		this.cell_range = cell_range;
	}


	public String getWindow_size() {
		return window_size;
	}


	public void setWindow_size(String window_size) {
		this.window_size = window_size;
	}


	public String getSlides() {
		return slides;
	}


	public void setSlides(String slides) {
		this.slides = slides;
	}


	public String getEffect() {
		return effect;
	}


	public void setEffect(String effect) {
		this.effect = effect;
	}


	public String getId() {
		return id;
	}


	public void setId(String id) {
		this.id = id;
	}


	public String getTitle() {
		return title;
	}


	public void setTitle(String title) {
		this.title = title;
	}


	public String getDescription() {
		return description;
	}


	public void setDescription(String description) {
		this.description = description;
	}


	public String getBrowser_url() {
		return browser_url;
	}


	public void setBrowser_url(String browser_url) {
		this.browser_url = browser_url;
	}


	public String getEventQual() {
		return eventQual;
	}


	public void setEventQual(String eventQual) {
		this.eventQual = eventQual;
	}


	public String getTab_moved_from_index() {
		return tab_moved_from_index;
	}


	public void setTab_moved_from_index(String tab_moved_from_index) {
		this.tab_moved_from_index = tab_moved_from_index;
	}


	public String getTab_moved_to_index() {
		return tab_moved_to_index;
	}


	public void setTab_moved_to_index(String tab_moved_to_index) {
		this.tab_moved_to_index = tab_moved_to_index;
	}


	public String getNewZoomFactor() {
		return newZoomFactor;
	}


	public void setNewZoomFactor(String newZoomFactor) {
		this.newZoomFactor = newZoomFactor;
	}


	public String getOldZoomFactor() {
		return oldZoomFactor;
	}


	public void setOldZoomFactor(String oldZoomFactor) {
		this.oldZoomFactor = oldZoomFactor;
	}


	public String getTab_pinned() {
		return tab_pinned;
	}


	public void setTab_pinned(String tab_pinned) {
		this.tab_pinned = tab_pinned;
	}


	public String getTab_audible() {
		return tab_audible;
	}


	public void setTab_audible(String tab_audible) {
		this.tab_audible = tab_audible;
	}


	public String getTab_muted() {
		return tab_muted;
	}


	public void setTab_muted(String tab_muted) {
		this.tab_muted = tab_muted;
	}


	public String getWindow_ingognito() {
		return window_ingognito;
	}


	public void setWindow_ingognito(String window_ingognito) {
		this.window_ingognito = window_ingognito;
	}


	public String getFile_size() {
		return file_size;
	}


	public void setFile_size(String file_size) {
		this.file_size = file_size;
	}


	public String getTag_category() {
		return tag_category;
	}


	public void setTag_category(String tag_category) {
		this.tag_category = tag_category;
	}


	public String getTag_type() {
		return tag_type;
	}


	public void setTag_type(String tag_type) {
		this.tag_type = tag_type;
	}


	public String getTag_name() {
		return tag_name;
	}


	public void setTag_name(String tag_name) {
		this.tag_name = tag_name;
	}


	public String getTag_title() {
		return tag_title;
	}


	public void setTag_title(String tag_title) {
		this.tag_title = tag_title;
	}


	public String getTag_value() {
		return tag_value;
	}


	public void setTag_value(String tag_value) {
		this.tag_value = tag_value;
	}


	public String getTag_checked() {
		return tag_checked;
	}


	public void setTag_checked(String tag_checked) {
		this.tag_checked = tag_checked;
	}


	public String getTag_html() {
		return tag_html;
	}


	public void setTag_html(String tag_html) {
		this.tag_html = tag_html;
	}


	public String getTag_href() {
		return tag_href;
	}


	public void setTag_href(String tag_href) {
		this.tag_href = tag_href;
	}


	public String getTag_innerText() {
		return tag_innerText;
	}


	public void setTag_innerText(String tag_innerText) {
		this.tag_innerText = tag_innerText;
	}


	public String getTag_option() {
		return tag_option;
	}


	public void setTag_option(String tag_option) {
		this.tag_option = tag_option;
	}


	@Override
	public String toString() {
		return "UserAction [timestamp=" + timestamp + ", user=" + user + ", category=" + category + ", application="
				+ application + ", event_type=" + event_type + ", event_src_path=" + event_src_path
				+ ", event_dest_path=" + event_dest_path + ", clipboard_content=" + clipboard_content + ", workbook="
				+ workbook + ", current_worksheet=" + current_worksheet + ", worksheets=" + worksheets + ", sheets="
				+ sheets + ", cell_content=" + cell_content + ", cell_range=" + cell_range + ", window_size="
				+ window_size + ", slides=" + slides + ", effect=" + effect + ", id=" + id + ", title=" + title
				+ ", description=" + description + ", browser_url=" + browser_url + ", eventQual=" + eventQual
				+ ", tab_moved_from_index=" + tab_moved_from_index + ", tab_moved_to_index=" + tab_moved_to_index
				+ ", newZoomFactor=" + newZoomFactor + ", oldZoomFactor=" + oldZoomFactor + ", tab_pinned=" + tab_pinned
				+ ", tab_audible=" + tab_audible + ", tab_muted=" + tab_muted + ", window_ingognito=" + window_ingognito
				+ ", file_size=" + file_size + ", tag_category=" + tag_category + ", tag_type=" + tag_type
				+ ", tag_name=" + tag_name + ", tag_title=" + tag_title + ", tag_value=" + tag_value + ", tag_checked="
				+ tag_checked + ", tag_html=" + tag_html + ", tag_href=" + tag_href + ", tag_innerText=" + tag_innerText
				+ ", tag_option=" + tag_option + "]";
	}
	
	
	

}
