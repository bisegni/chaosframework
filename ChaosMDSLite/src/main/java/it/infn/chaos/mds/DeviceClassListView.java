/**
 * 
 */
package it.infn.chaos.mds;

import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.DeviceClass;
import it.infn.chaos.mds.business.UnitServerCuInstance;

import java.util.Collection;
import java.util.Observable;
import java.util.Set;
import java.util.Vector;

import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.webapp.RefVaadinBasePanel;
import org.ref.server.webapp.dialog.RefVaadinErrorDialog;
import org.vaadin.dialogs.ConfirmDialog;

import com.vaadin.data.Item;
import com.vaadin.data.Property.ValueChangeEvent;
import com.vaadin.event.Action;
import com.vaadin.event.ItemClickEvent;
import com.vaadin.event.ItemClickEvent.ItemClickListener;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.Table;
import com.vaadin.ui.Window;
import com.vaadin.ui.Window.Notification;

/**
 * @author Andrea Michelotti
 * 
 */
public class DeviceClassListView extends RefVaadinBasePanel implements com.vaadin.data.Container.PropertySetChangeListener, ItemClickListener {
	private static final long				serialVersionUID				= -841710385419733478L;
	static final Action						ACTION_EDIT						= new Action("Edit");
	static final Action						ACTION_SAVE						= new Action("Save");
	static final Action[]					ACTIONS_TO_EDIT					= new Action[] { ACTION_EDIT };
	static final Action[]					ACTIONS_IN_EDIT					= new Action[] { ACTION_SAVE };

	private static final Object				TABLE_COLUMN_CLASS_ALIAS		= "Class Name ID";
	private static final Object				TABLE_COLUMN_CLASS_NAME			= "CU Class";
	private static final Object				TABLE_COLUMN_CLASS_INTERFACE	= "CU Interface";
	private static final Object				TABLE_COLUMN_CLASS_SOURCE		= "Source";
	private static final Object				TABLE_COLUMN_CLASS_UID			= "UID";

	private DeviceClassListVaadin			impl							= new DeviceClassListVaadin();
	private boolean							editingServer					= false;
	private Vector<DeviceClass>				deviceClasses					= null;
	private Vector<DeviceClass>				deviceClassesModified			= new Vector<DeviceClass>();
	private Vector<DeviceClass>				deviceClassesAdded				= new Vector<DeviceClass>();

	protected int							idx=0;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#initGui()
	 */
	@SuppressWarnings("serial")
	@Override
	public void initGui() {
		addComponent(impl);
	//	impl.getTableAssociation().addListener((ItemClickListener) this);
		impl.getTableAssociation().setSelectable(true);
		// impl.getTableAssociation().setImmediate(true);
		// impl.getTableAssociation().setReadThrough(true);
		impl.getTableAssociation().setMultiSelect(false);
		impl.getTableAssociation().setNullSelectionAllowed(true);
		impl.getTableAssociation().addContainerProperty(DeviceClassListView.TABLE_COLUMN_CLASS_ALIAS, String.class, null);
		impl.getTableAssociation().addContainerProperty(DeviceClassListView.TABLE_COLUMN_CLASS_NAME, String.class, null);
		impl.getTableAssociation().addContainerProperty(DeviceClassListView.TABLE_COLUMN_CLASS_INTERFACE, String.class, null);
		impl.getTableAssociation().addContainerProperty(DeviceClassListView.TABLE_COLUMN_CLASS_SOURCE, String.class, null);
		impl.getTableAssociation().addContainerProperty(DeviceClassListView.TABLE_COLUMN_CLASS_UID, Integer.class, null);

		impl.getTableAssociation().addActionHandler(new Action.Handler() {
			public Action[] getActions(Object target, Object sender) {
				return new Action[] { ACTION_EDIT,ACTION_SAVE };
			}

			public void handleAction(Action action, Object sender, Object target) {
				if(impl.getTableAssociation().getValue() == null)
					return;
				
			
				Integer c = (Integer)impl.getTableAssociation().getValue();
				Item item = impl.getTableAssociation().getItem(c);
				if (ACTION_EDIT == action) {
					impl.getTableAssociation().setEditable(true);
					impl.getTableAssociation().refreshRowCache();
					item.getItemProperty(TABLE_COLUMN_CLASS_UID).setReadOnly(true);
					item.getItemProperty(TABLE_COLUMN_CLASS_SOURCE).setReadOnly(true);	
				} else if (ACTION_SAVE == action) {
					impl.getTableAssociation().refreshRowCache();
					impl.getTableAssociation().setEditable(false);
					impl.getButtonSave().setEnabled(true);
					DeviceClass dc = new DeviceClass();
					
					int cnt=0;
					
					
					dc.setDeviceClass(item.getItemProperty(TABLE_COLUMN_CLASS_NAME).toString());
					dc.setDeviceClassAlias(item.getItemProperty(TABLE_COLUMN_CLASS_ALIAS).toString());
					dc.setDeviceClassInterface(item.getItemProperty(TABLE_COLUMN_CLASS_INTERFACE).toString());
					dc.setDeviceSource("MDS");
					if(item.getItemProperty(TABLE_COLUMN_CLASS_UID).toString() == null){
						dc.setClassId(null);
					} else {
						dc.setClassId(Integer.valueOf(item.getItemProperty(TABLE_COLUMN_CLASS_UID).toString()));
					}
					for(DeviceClass cc:deviceClasses){
						if(cc.getDeviceClass()==dc.getDeviceClass()){
							cnt++;
						}
						if(cnt>1){
							deviceClasses.remove(c);
							getWindow().showNotification("Class Name", "<b>must be unique</b>", Notification.TYPE_ERROR_MESSAGE);
							return;
						}
					}
					deviceClassesModified.remove(c);
					deviceClassesModified.add(dc);
					
				} 
			}

		});
		
		
		
		impl.getButtonSave().addListener(new ClickListener(){
			public void buttonClick(ClickEvent event){
				impl.getButtonSave().setEnabled(false);

				 for(DeviceClass dc:deviceClassesModified){
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_SAVE_DEVICE_CLASS, event.getSource(), dc);	 
				 }
				 deviceClassesModified.removeAllElements();
				 
			}
		});
		
		impl.getButtonDel().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				Integer c = (Integer) impl.getTableAssociation().getValue();
				impl.getTableAssociation().removeItem(c);
				DeviceClass cd = deviceClasses.get(c);
				deviceClasses.remove(c);
				notifyEventoToControllerWithData(MDSUIEvents.EVENT_REMOVE_DEVICE_CLASS, event.getSource(), cd);
			
			}
		});
		
		impl.getButtonNew().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				Item item = impl.getTableAssociation().addItem(new Integer(idx));
				item.getItemProperty(TABLE_COLUMN_CLASS_ALIAS).setValue("<alias name>");
				item.getItemProperty(TABLE_COLUMN_CLASS_NAME).setValue("<CU C++ TYPE>");
				item.getItemProperty(TABLE_COLUMN_CLASS_INTERFACE).setValue("<class interface >");
				item.getItemProperty(TABLE_COLUMN_CLASS_SOURCE).setValue("---");
				item.getItemProperty(TABLE_COLUMN_CLASS_UID).setValue(null);
				deviceClasses.add(idx++,new DeviceClass());
			}
		});
		
		impl.getButtonCloseView().addListener(new ClickListener() {
			public void buttonClick(ClickEvent event) {
				if(!deviceClassesModified.isEmpty()){
					ConfirmDialog.show(getWindow(), "Do you want quit?, you have pending modifications",
					        new ConfirmDialog.Listener() {
								ClickEvent my_ev;

					            public void onClose(ConfirmDialog dialog) {
					                if (dialog.isConfirmed()) {
										notifyEventoToControllerWithData(MDSUIEvents.EVENT_CLOSE_DEVICE_CLASS_VIEW, my_ev.getSource(),null);
					                } 
					                
					            }
					            private ConfirmDialog.Listener init(ClickEvent ev){my_ev = ev; return this;}
					        }.init(event));
				} else {
					notifyEventoToControllerWithData(MDSUIEvents.EVENT_CLOSE_DEVICE_CLASS_VIEW, event.getSource(),null);
				}

			}
		});
			}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinBasePanel#update(java.util.Observable, java.lang.Object)
	 */
	@Override
	public void update(Observable source, Object sourceData) {
		ViewNotifyEvent viewEvent = null;
		impl.getButtonSave().setEnabled(false);

		if (sourceData instanceof ViewNotifyEvent) {
			viewEvent = (ViewNotifyEvent) sourceData;
			if (viewEvent.getEventKind().equals(MDSUIEvents.EVENT_UPDATE_DEVICE_CLASS)) {
				impl.getTableAssociation().removeAllItems();
				idx = 0;
				deviceClassesModified.removeAllElements();
				deviceClasses = (Vector<DeviceClass>) viewEvent.getEventData();
				if (deviceClasses == null)
					return;
				for (DeviceClass c : deviceClasses) {
					Item woItem = impl.getTableAssociation().addItem(new Integer(idx++));
					woItem.getItemProperty(DeviceClassListView.TABLE_COLUMN_CLASS_ALIAS).setValue(c.getDeviceClassAlias());
					woItem.getItemProperty(DeviceClassListView.TABLE_COLUMN_CLASS_NAME).setValue(c.getDeviceClass());
					woItem.getItemProperty(DeviceClassListView.TABLE_COLUMN_CLASS_INTERFACE).setValue(c.getDeviceClassInterface());
					woItem.getItemProperty(DeviceClassListView.TABLE_COLUMN_CLASS_SOURCE).setValue(c.getDeviceSource());
					woItem.getItemProperty(DeviceClassListView.TABLE_COLUMN_CLASS_UID).setValue(c.getClassId());

				}

			} 
		}
	}

	/*
	 * (non-Javadoc)
	 * 
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

	@Override
	public void itemClick(ItemClickEvent event) {
		// TODO Auto-generated method stub
		
	}


	
}
