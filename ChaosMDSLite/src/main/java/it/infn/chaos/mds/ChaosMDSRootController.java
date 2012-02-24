package it.infn.chaos.mds;

import it.infn.chaos.mds.business.DataServer;
import it.infn.chaos.mds.business.Dataset;
import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.process.ManageDeviceProcess;
import it.infn.chaos.mds.process.ManageServerProcess;

import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Observable;

import org.ref.common.exception.RefException;
import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.auth.RefAuthneticatorListener;
import org.ref.server.webapp.RefVaadinApplicationController;
import org.ref.server.webapp.RefVaadinMenuManager;
import org.ref.server.webapp.dialog.RefVaadinErrorDialog;

import com.vaadin.data.Item;
import com.vaadin.data.util.BeanContainer;
import com.vaadin.ui.Component;
import com.vaadin.ui.Table;
import com.vaadin.ui.UriFragmentUtility.FragmentChangedEvent;

@SuppressWarnings("serial")
public class ChaosMDSRootController extends RefVaadinApplicationController implements RefAuthneticatorListener {
	private ManageDeviceProcess	mdp	= null;
	private ManageServerProcess	msp	= null;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#initController()
	 */
	@Override
	public void initController() throws Throwable {
		mdp = (ManageDeviceProcess) openProcess(ManageDeviceProcess.class.getSimpleName(), null);
		msp = (ManageServerProcess) openProcess(ManageServerProcess.class.getSimpleName(), null);
		openViewByKeyAndClass("VISTA", DemoAppView.class);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#updateMainView(org
	 * .ref.server.webapp .RefVaadinApplicationController, com.vaadin.ui.Component)
	 */
	@Override
	public void updateMainView(RefVaadinApplicationController controller, Component applicationViewComponent) {
		try {
			if (applicationViewComponent.equals(getViewByKey("VISTA"))) {
				updateDeviceList();
			} else if (applicationViewComponent.equals(getViewByKey("VISTA_DUE"))) {
				notifyEventoToViewWithData(LiveDataPreferenceView.EVENT_PREFERENCE_UPDATE_SERVER_LIST, this, msp.getAllServer());
			}
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	/**
	 * 
	 */
	public void updateMainView(String key, RefVaadinApplicationController controller, Component applicationViewComponent) {
		updateMainView(controller, applicationViewComponent);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#update(java.util .Observable,
	 * java.lang.Object)
	 */
	@Override
	public void update(Observable source, Object sourceData) {
		if (sourceData == null)
			return;
		try {
			ViewNotifyEvent viewEvent = null;

			if (sourceData instanceof ViewNotifyEvent) {
				viewEvent = (ViewNotifyEvent) sourceData;
				if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_CLOSE_VIEW)) {
					
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_INITIALIZE_DEVICE_AT_STARTUP)) {
					initializedOptionForSelectedDevice(true);
				} else if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_ADD_NEW_SERVER)) {
					msp.addNewServer();
					notifyEventoToViewWithData(LiveDataPreferenceView.EVENT_PREFERENCE_UPDATE_SERVER_LIST, this, msp.getAllServer());
				} else if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_DELETE_SERVER)) {
					deleteSeletedDataserver();
				}else if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_UPDATE_SERVER_DATA)) {
					updateServerData();
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_DEVICE_SELECTED)) {
					deviceHasBeenSelected(viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_UPDATE_DEVICE_LIST)) {
					updateDeviceList();
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_DATASET_SELECTED)) {
					updateDatasetAttributeList((Dataset) viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_ATTRIBUTE_SAVE)) {
					updateAttributeUserModification();
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_DELETE_SELECTED_DEVICE)) {
					deleteSelectedDevice();
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_SHOW_PREFERENCE)) {
					try {
						openViewByKeyAndClass("VISTA_DUE", LiveDataPreferenceView.class, OpenViewIn.DIALOG, "Live Data Preference");
					} catch (InstantiationException e) {
						e.printStackTrace();
					} catch (IllegalAccessException e) {
						e.printStackTrace();
					}
				}
			}
		} catch (Throwable e) {
			DemoAppView view = getViewByKey("VISTA");
			RefVaadinErrorDialog.shorError(view.getWindow(), "Event Error", e.getMessage());
		}
	}

	/**
	 * @throws Throwable 
	 * 
	 */
	private void deleteSeletedDataserver() throws Throwable {
		LiveDataPreferenceView view = getViewByKey("VISTA_DUE");
		Table t = (Table) view.getComponentByKey(LiveDataPreferenceView.KEY_PREFERENCE_TABLE);
		@SuppressWarnings("unchecked")
		Collection<Integer> dataServer = (Collection<Integer>) t.getValue();
		for (Iterator<Integer> iterator = dataServer.iterator(); iterator.hasNext();) {
			Integer dsIDToUpdate = (Integer) iterator.next();
			@SuppressWarnings("unchecked")
			DataServer dsToUpdate = ((BeanContainer<Integer, DataServer>) t.getContainerDataSource()).getItem(dsIDToUpdate).getBean();
			msp.deleteServer(dsToUpdate);
		}
		notifyEventoToViewWithData(LiveDataPreferenceView.EVENT_PREFERENCE_UPDATE_SERVER_LIST, this, msp.getAllServer());
		
	}

	/**
	 * 
	 * @param initialize
	 * @throws Throwable 
	 */
	private void initializedOptionForSelectedDevice(boolean initialize) throws Throwable {
		DemoAppView view = getViewByKey("VISTA");
		Table dsTable = (Table) view.getComponentByKey(DemoAppView.KEY_DEVICE_TAB);
		@SuppressWarnings("unchecked")
		Collection<String> selectedDevice = (Collection<String>) dsTable.getItemIds();
		for (Iterator<String> iterator = selectedDevice.iterator(); iterator.hasNext();) {
			String deviceIdentification = iterator.next();
			mdp.swapDeviceStartupInitizializationOption(deviceIdentification);
		}
		updateDeviceList();
	}

	private void updateServerData() throws Throwable {
		LiveDataPreferenceView view = getViewByKey("VISTA_DUE");
		Table t = (Table) view.getComponentByKey(LiveDataPreferenceView.KEY_PREFERENCE_TABLE);
		@SuppressWarnings("unchecked")
		Collection<Integer> dataServer = (Collection<Integer>) t.getItemIds();
		for (Iterator<Integer> iterator = dataServer.iterator(); iterator.hasNext();) {
			Integer dsIDToUpdate = (Integer) iterator.next();
			@SuppressWarnings("unchecked")
			DataServer dsToUpdate = ((BeanContainer<Integer, DataServer>) t.getContainerDataSource()).getItem(dsIDToUpdate).getBean();
			msp.updateDataServer(dsToUpdate);
		}
	}

	/**
	 * @throws Throwable
	 * 
	 */
	private void deleteSelectedDevice() throws Throwable {
		DemoAppView view = getViewByKey("VISTA");
		Table t = (Table) view.getComponentByKey(DemoAppView.KEY_DEVICE_TAB);
		Object o = t.getValue();
		mdp.deleteDeviceByInstance(o.toString());
		updateDeviceList();
	}

	/**
	 * @throws RefException
	 */
	private void updateAttributeUserModification() throws RefException {
		DemoAppView view = getViewByKey("VISTA");
		try {
			Table t = (Table) view.getComponentByKey(DemoAppView.KEY_DATASET_ATTRIBUTE_TAB);
			@SuppressWarnings("unchecked")
			Collection<DatasetAttribute> datasetAttributes = (Collection<DatasetAttribute>) t.getItemIds();
			for (DatasetAttribute datasetAttributeItemID : datasetAttributes) {
				Object tmpValue = null;
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(DemoAppView.TAB3_TAGS_PATH);
				if (tmpValue != null) {
					datasetAttributeItemID.setTagsPath(tmpValue.toString());
				}
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(DemoAppView.TAB3_RANG_MIN);
				if (tmpValue != null) {
					datasetAttributeItemID.setRangeMin(tmpValue.toString());
				}
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(DemoAppView.TAB3_RANG_MAX);
				if (tmpValue != null) {
					datasetAttributeItemID.setRangeMax(tmpValue.toString());
				}
				 tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(DemoAppView.TAB3_DEF_VALUE);
					if (tmpValue != null) {
						datasetAttributeItemID.setDefaultValue(tmpValue.toString());
					}
			}
			mdp.updateDeviceAttributes(datasetAttributes);
		} catch (Throwable e) {
			RefVaadinErrorDialog.shorError(view.getWindow(), "Update Attribute Error", e.getMessage());
			notifyEventoToViewWithData(DemoAppView.EVENT_ATTRIBUTE_EDITING, null, null);
		}
	}

	/**
	 * @throws Throwable
	 */
	private void updateDatasetAttributeList(Dataset dsAttr) throws Throwable {
		DemoAppView view = getViewByKey("VISTA");
		Table dsAttrTable = (Table) view.getComponentByKey(DemoAppView.KEY_DATASET_ATTRIBUTE_TAB);
		dsAttrTable.removeAllItems();
		if (dsAttr == null)
			return;
		List<DatasetAttribute> datasetsAttributeForDataset = mdp.getAttributeForDataset(dsAttr);
		for (DatasetAttribute dsAt : datasetsAttributeForDataset) {
			Item woItem = dsAttrTable.addItem(dsAt);
			woItem.getItemProperty(DemoAppView.TAB3_NAME).setValue(dsAt.getName());
			woItem.getItemProperty(DemoAppView.TAB3_DIR).setValue(dsAt.getNamedDirection());
			woItem.getItemProperty(DemoAppView.TAB3_TYPE).setValue(dsAt.getNamedType());
			woItem.getItemProperty(DemoAppView.TAB3_RANG_MIN).setValue(dsAt.getRangeMin());
			woItem.getItemProperty(DemoAppView.TAB3_RANG_MAX).setValue(dsAt.getRangeMax());
			woItem.getItemProperty(DemoAppView.TAB3_DEF_VALUE).setValue(dsAt.getDefaultValue());
			woItem.getItemProperty(DemoAppView.TAB3_TAGS_PATH).setValue(dsAt.getTagsPath());
		}
	}

	/**
	 * A device has been selected so we need to update the datasets
	 * 
	 * @param deviceID
	 * @throws Throwable
	 */
	private void deviceHasBeenSelected(Object deviceIdentification) throws Throwable {
		DemoAppView view = getViewByKey("VISTA");
		Table dsTable = (Table) view.getComponentByKey(DemoAppView.KEY_DATASET_TAB);
		//Button dsButtonInitiAtStartup = (Button) view.getComponentByKey(DemoAppView.KEY_DEVICE_START_AT_INIT_BUTTON);

		dsTable.removeAllItems();
		updateDatasetAttributeList(null);
		if (deviceIdentification == null)
			return;

		//dsButtonInitiAtStartup.setEnabled(!mdp.isDeviceInitializableAtStartup(deviceIdentification.toString()));

		List<Dataset> datasetsForDevice = mdp.getAllDatasetForDeviceIdentification(deviceIdentification.toString());

		for (Dataset ds : datasetsForDevice) {
			Item woItem = dsTable.addItem(ds);
			woItem.getItemProperty(DemoAppView.TAB2_TIMESTAMP).setValue(ds.getTimestamp().getDate());
			woItem.getItemProperty(DemoAppView.TAB2_ATTR_NUMBER).setValue(ds.getAttributes() != null ? ds.getAttributes().size() : 0);
		}
	}

	/**
	 * @throws Throwable
	 */
	private void updateDeviceList() throws Throwable {
		deviceHasBeenSelected(null);
		DemoAppView view = getViewByKey("VISTA");
		// load all dataset
		List<Device> allDevices = mdp.getAlDevices();
		Table t = (Table) view.getComponentByKey(DemoAppView.KEY_DEVICE_TAB);
		t.removeAllItems();
		for (Device device : allDevices) {
			Item woItem = t.addItem(device.getDeviceIdentification());
			woItem.getItemProperty(DemoAppView.TAB1_DEVICE_INSTANCE).setValue(device.getDeviceIdentification());
			woItem.getItemProperty(DemoAppView.TAB1_CU_PARENT).setValue(device.getCuInstance());
			woItem.getItemProperty(DemoAppView.TAB1_NET_ADDRESS).setValue(device.getNetAddress());
			woItem.getItemProperty(DemoAppView.TAB1_LAST_HB).setValue(device.getLastHeartBeatTimestamp() != null ? device.getLastHeartBeatTimestamp().getDate() : null);
			woItem.getItemProperty("INIT").setValue(device.getInitAtStartup());
		}

	}

	/*
	 * (non-Javadoc)
	 * @see org.ref.server.auth.RefAuthneticatorListener#loggedOut(java.lang.Object[])
	 */
	public void loggedOut(Object... infoData) {
	}

	/*
	 * (non-Javadoc)
	 * @see org.ref.server.auth.RefAuthneticatorListener#loggedIn(java.lang.Object[])
	 */
	public void loggedIn(Object... infoData) {
	}

	@Override
	public void fragmentChanged(FragmentChangedEvent source) {

	}

	protected void manageFragmentParameters(String[] parts) {
	}

	public void setMenuManager(RefVaadinMenuManager rootWindow) {
	}

	/*
	 * (non-Javadoc)
	 * @see com.vaadin.terminal.ParameterHandler#handleParameters(java.util.Map)
	 */
	public void handleParameters(Map<String, String[]> parameters) {
		// TODO Auto-generated method stub

	}
}
