/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.business.DataServer;

import java.util.List;
import java.util.Observable;
import java.util.Vector;

import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.webapp.RefVaadinBasePanel;

import com.vaadin.data.Item;
import com.vaadin.data.util.BeanContainer;
import com.vaadin.event.Action;
import com.vaadin.ui.Upload;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.Upload.SucceededEvent;

/**
 * @author bisegni
 * 
 */
public class LiveDataPreferenceView extends RefVaadinBasePanel implements com.vaadin.data.Container.PropertySetChangeListener {
	private static final long					serialVersionUID					= -841710385419733478L;
	public static final String					KEY_PREFERENCE_TABLE				= "KEY_PREFERENCE_TABLE";
	public static final String					EVENT_PREFERENCE_ADD_NEW_SERVER		= "event_preference_add_new_server";
	public static final String					EVENT_PREFERENCE_CLOSE_VIEW			= "event_preference_close_view";
	public static final String					EVENT_PREFERENCE_DELETE_SERVER		= "event_preference_delete_server";
	public static final String					EVENT_PREFERENCE_UPDATE_SERVER_LIST	= "event_preference_update_server_list";
	public static final String					EVENT_PREFERENCE_UPDATE_SERVER_DATA	= "event_preference_update_server_data";
	
	public static final String					CONFIG_REPLACE_ALL					= "Replace All";
	public static final String					CONFIG_KEEP_SERVER					= "Keep Server";
	public static final String					CONFIG_KEEP_CONFIG					= "Keep Config";
	public static final String					CONFIG_KEEP_BOTH					= "Keep Both (renaming conflits)";
	public static final String					CONFIG_DIR							="/tmp/MDS/";
	static final Action							ACTION_EDIT							= new Action("Edit");
	static final Action							ACTION_SAVE							= new Action("Save");
	static final Action[]						ACTIONS_TO_EDIT						= new Action[] { ACTION_EDIT };
	static final Action[]						ACTIONS_IN_EDIT						= new Action[] { ACTION_SAVE };
	private LiveDataPreferenceVaadin			liveDataView						= new LiveDataPreferenceVaadin();
	private BeanContainer<Integer, DataServer>	serverListBeanContainer				= new BeanContainer<Integer, DataServer>(DataServer.class);
	private boolean								editingServer						= false;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#initGui()
	 */
	@SuppressWarnings("serial")
	@Override
	public void initGui() {
		addComponent(liveDataView);
		liveDataView.getPreferenceTable().setEditable(false);
		liveDataView.getPreferenceTable().setSelectable(true);
		liveDataView.getPreferenceTable().setMultiSelect(true);
		serverListBeanContainer.addListener(this);
		setComponentKey(KEY_PREFERENCE_TABLE, liveDataView.getPreferenceTable());
		serverListBeanContainer.setBeanIdProperty("idServer");
		liveDataView.getPreferenceTable().setContainerDataSource(serverListBeanContainer);
		liveDataView.getPreferenceTable().setVisibleColumns(new Object[] { "hostname", "isLiveServer" });
		liveDataView.getPreferenceTable().setColumnHeaders(new String[] { "Hostname", "Live" });
		liveDataView.getPreferenceTable().addActionHandler(new Action.Handler() {
			public Action[] getActions(Object target, Object sender) {
				return isEditingServer() ? ACTIONS_IN_EDIT : ACTIONS_TO_EDIT;
			}

			public void handleAction(Action action, Object sender, Object target) {
				setEditingServer(!isEditingServer());
				if (ACTION_EDIT == action) {
					liveDataView.getPreferenceTable().setEditable(!liveDataView.getPreferenceTable().isEditable());
					liveDataView.getPreferenceTable().refreshRowCache();
				} else if (ACTION_SAVE == action) {
					liveDataView.getPreferenceTable().setEditable(!liveDataView.getPreferenceTable().isEditable());
					liveDataView.getPreferenceTable().refreshRowCache();
					notifyEventoToControllerWithData(EVENT_PREFERENCE_UPDATE_SERVER_DATA, null, null);

				}
			}

		});
		liveDataView.getButtonClose().addListener(new ClickListener() {
			/*
			 * (non-Javadoc)
			 * @see com.vaadin.ui.Button.ClickListener#buttonClick(com.vaadin.ui.Button.ClickEvent)
			 */
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_PREFERENCE_CLOSE_VIEW, event.getSource(), null);
			}

		});
		liveDataView.getButtonDelete().addListener(new ClickListener() {
			/*
			 * (non-Javadoc)
			 * @see com.vaadin.ui.Button.ClickListener#buttonClick(com.vaadin.ui.Button.ClickEvent)
			 */
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_PREFERENCE_DELETE_SERVER, event.getSource(), null);
			}

		});
		liveDataView.getButtonInsert().addListener(new ClickListener() {
			/*
			 * (non-Javadoc)
			 * @see com.vaadin.ui.Button.ClickListener#buttonClick(com.vaadin.ui.Button.ClickEvent)
			 */
			public void buttonClick(ClickEvent event) {
				notifyEventoToControllerWithData(EVENT_PREFERENCE_ADD_NEW_SERVER, event.getSource(), null);
			}

		});
		liveDataView.getReplaceOption().setImmediate(true);
		liveDataView.getReplaceOption().setNullSelectionAllowed(false);
	/*	liveDataView.getReplaceOption().addItem(CONFIG_REPLACE_ALL);
		liveDataView.getReplaceOption().addItem(CONFIG_KEEP_SERVER);
		liveDataView.getReplaceOption().addItem(CONFIG_KEEP_CONFIG);
		liveDataView.getReplaceOption().addItem(CONFIG_KEEP_BOTH);
		*/
		liveDataView.getReplaceOption().addItem(0);
		liveDataView.getReplaceOption().addItem(1);
		liveDataView.getReplaceOption().addItem(2);
		liveDataView.getReplaceOption().addItem(3);
		liveDataView.getReplaceOption().setItemCaption(0, CONFIG_REPLACE_ALL);
		liveDataView.getReplaceOption().setItemCaption(1, CONFIG_KEEP_SERVER);
		liveDataView.getReplaceOption().setItemCaption(2, CONFIG_KEEP_CONFIG);
		liveDataView.getReplaceOption().setItemCaption(3, CONFIG_KEEP_BOTH);
/*
		liveDataView.getReplaceOption().addContainerProperty(CONFIG_REPLACE_ALL, Integer.class, 0);
		liveDataView.getReplaceOption().addContainerProperty(CONFIG_KEEP_SERVER, Integer.class, 1);
		liveDataView.getReplaceOption().addContainerProperty(CONFIG_KEEP_CONFIG, Integer.class, 2);
		liveDataView.getReplaceOption().addContainerProperty(CONFIG_KEEP_BOTH,   Integer.class, 3);
		*/
		
		
		liveDataView.getReplaceOption().select(0);
	/////////////////////////////////////
	/////// DOWNLOAD
	
	liveDataView.getButtonDownload().addListener(new ClickListener(){
	public void buttonClick(ClickEvent event) {	
	notifyEventoToControllerWithData(MDSUIEvents.EVENT_DUMP_CONFIGURATION, event, null);
	
	}
	});
	//// UPLOAD
	FileUploader fup=new FileUploader(CONFIG_DIR);
	liveDataView.getUpload().setReceiver(fup);
	
	liveDataView.getUpload().addListener(new Upload.SucceededListener() {
	
	@Override
	public void uploadSucceeded(SucceededEvent event) {
		Vector<Object> param = new Vector<Object>();
		param.add(CONFIG_DIR + event.getFilename());		
		param.add(liveDataView.getReplaceOption().getValue());
		notifyEventoToControllerWithData(MDSUIEvents.EVENT_APPLY_CONFIGURATION, event, param);				
	}
	});

////

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#update(java.util.Observable, java.lang.Object)
	 */
	@Override
	public void update(Observable source, Object sourceData) {
		ViewNotifyEvent viewEvent = (ViewNotifyEvent) sourceData;
		if (viewEvent.getEventKind().equals(EVENT_PREFERENCE_UPDATE_SERVER_LIST)
			&& viewEvent.getEventData() != null) {
			@SuppressWarnings("unchecked")
			List<DataServer> listDataServer = (List<DataServer>) viewEvent.getEventData();
			serverListBeanContainer.removeAllItems();
			serverListBeanContainer.addAll(listDataServer);
			// liveDataView.getPreferenceTable().setContainerDataSource(beans);
		}
	}

	/*
	 * (non-Javadoc)
	 * @see com.vaadin.data.Container.PropertySetChangeListener#containerPropertySetChange(com.vaadin.data.Container.PropertySetChangeEvent)
	 */
	public void containerPropertySetChange(com.vaadin.data.Container.PropertySetChangeEvent event) {
		//System.out.println(event);

	}

	/**
	 * @return the editingServer
	 */
	public boolean isEditingServer() {
		return editingServer;
	}

	/**
	 * @param editingServer
	 *            the editingServer to set
	 */
	public void setEditingServer(boolean editingServer) {
		this.editingServer = editingServer;
	}

}
