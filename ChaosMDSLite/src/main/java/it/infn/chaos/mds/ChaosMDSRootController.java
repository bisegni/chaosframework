package it.infn.chaos.mds;

import it.infn.chaos.mds.batchexecution.LoadUnloadWorkUnit;
import it.infn.chaos.mds.batchexecution.LoadUnloadWorkUnit.LoadUnloadWorkUnitSetting;
import it.infn.chaos.mds.business.DataServer;
import it.infn.chaos.mds.business.Dataset;
import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.business.UnitServerCuInstance;
import it.infn.chaos.mds.process.ManageDeviceProcess;
import it.infn.chaos.mds.process.ManageServerProcess;
import it.infn.chaos.mds.process.ManageUnitServerProcess;

import java.sql.SQLException;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Observable;
import java.util.Set;
import java.util.Vector;

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
	private ManageDeviceProcess		mdp		= null;
	private ManageServerProcess		msp		= null;
	private ManageUnitServerProcess	musp	= null;
	private String					unitServerSelected;
	private String					unitServerCUTypeSelected;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#initController()
	 */
	@Override
	public void initController() throws Throwable {
		mdp = (ManageDeviceProcess) openProcess(ManageDeviceProcess.class.getSimpleName(), null);
		msp = (ManageServerProcess) openProcess(ManageServerProcess.class.getSimpleName(), mdp.getProcessName());
		musp = (ManageUnitServerProcess) openProcess(ManageUnitServerProcess.class.getSimpleName(), mdp.getProcessName());
		openViewByKeyAndClass("VISTA", MDSAppView.class);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#updateMainView(org .ref.server.webapp .RefVaadinApplicationController, com.vaadin.ui.Component)
	 */
	@Override
	public void updateMainView(RefVaadinApplicationController controller, Component applicationViewComponent) {
		try {
			if (applicationViewComponent.equals(getViewByKey("VISTA"))) {
				updateUnitServerList();
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
	 * @see org.ref.server.webapp.RefVaadinApplicationController#update(java.util .Observable, java.lang.Object)
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

				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_INITIALIZE_DEVICE_AT_STARTUP)) {
					initializedOptionForSelectedDevice(true);
				} else if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_ADD_NEW_SERVER)) {
					msp.addNewServer();
					notifyEventoToViewWithData(LiveDataPreferenceView.EVENT_PREFERENCE_UPDATE_SERVER_LIST, this, msp.getAllServer());
				} else if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_DELETE_SERVER)) {
					deleteSeletedDataserver();
				} else if (viewEvent.getEventKind().equals(LiveDataPreferenceView.EVENT_PREFERENCE_UPDATE_SERVER_DATA)) {
					updateServerData();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_DEVICE_SELECTED)) {
					deviceHasBeenSelected(viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UPDATE_DEVICE_LIST)) {
					updateDeviceList();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_DATASET_SELECTED)) {
					updateDatasetAttributeList((Dataset) viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_ATTRIBUTE_SAVE)) {
					updateAttributeUserModification();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_DELETE_SELECTED_DEVICE)) {
					deleteSelectedDevice();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_SHOW_PREFERENCE)) {
					try {
						openViewByKeyAndClass("VISTA_DUE", LiveDataPreferenceView.class, OpenViewIn.DIALOG, "Live Data Preference");
					} catch (InstantiationException e) {
						e.printStackTrace();
					} catch (IllegalAccessException e) {
						e.printStackTrace();
					}
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_SELECTED)) {
					unitServerSelected = viewEvent.getEventData().toString();
					unitServerHasBeenSelected(viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_NEW)) {
					createNewUnitServer();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_DELETE)) {
					if (unitServerSelected == null) {
						MDSAppView view = getViewByKey("VISTA");
						RefVaadinErrorDialog.shorError(view.getWindow(), "Delete unit server", "Select one unit server");
					} else {
						deleteSelectedUnitServer();
					}
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_CU_TYPE_SELECTED)) {
					unitServerCUTypeSelected = viewEvent.getEventData().toString();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_CREATE_US_CU_ASSOCIATION)) {
					try {
						if (unitServerSelected != null && unitServerCUTypeSelected != null) {
							openViewByKeyAndClass("NEW_US_CU_ASSOCIATION", NewUSCUAssociationView.class, OpenViewIn.DIALOG, "New Unit server - Work Unit Association");
							String[] sccu = new String[2];
							sccu[0] = unitServerSelected;
							sccu[1] = unitServerCUTypeSelected;
							notifyEventoToViewWithData(NewUSCUAssociationView.SET_SC_CUTYPE_VALUE, this, sccu);
						} else {
							MDSAppView view = getViewByKey("VISTA");
							RefVaadinErrorDialog.shorError(view.getWindow(), "SC-CU Association error", "SC or CU type invalid association");
						}
					} catch (InstantiationException e) {
						e.printStackTrace();
					} catch (IllegalAccessException e) {
						e.printStackTrace();
					}
				} else if (viewEvent.getEventKind().equals(NewUSCUAssociationView.EVENT_SAVE_USCU_ASSOC_VIEW)) {
					deleteViewByKey("NEW_US_CU_ASSOCIATION");
					UnitServerCuInstance usci = (UnitServerCuInstance) viewEvent.getEventData();
					musp.addUSCUAssociation(usci);
				} else if (viewEvent.getEventKind().equals(NewUSCUAssociationView.EVENT_CANCEL_USCU_ASSOC_VIEW)) {
					deleteViewByKey("NEW_US_CU_ASSOCIATION");
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_SHOW_ALL_ASSOCIATION)) {
					showAllAssociationForUnitServer();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_LOAD_ALL_ASSOCIATION)) {
					if (unitServerSelected != null) {
						loadUnloadAllUnitServerAssociation(true);
					} else {
						MDSAppView view = getViewByKey("VISTA");
						RefVaadinErrorDialog.shorError(view.getWindow(), "Load all association", "A unit server need to be selected");
					}
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_UNLOAD_ALL_ASSOCIATION)) {
					if (unitServerSelected != null) {
						loadUnloadAllUnitServerAssociation(false);
					} else {
						MDSAppView view = getViewByKey("VISTA");
						RefVaadinErrorDialog.shorError(view.getWindow(), "Unload all association", "A unit server need to be selected");
					}
				} else if (viewEvent.getEventKind().equals(USCUAssociationListView.EVENT_UPDATE_LIST)) {
					notifyEventoToViewWithData(USCUAssociationListView.EVENT_UPDATE_LIST, this, musp.loadAllAssociationForUnitServerAlias(unitServerSelected));
				} else if (viewEvent.getEventKind().equals(USCUAssociationListView.EVENT_REMOVE_ASSOCIATION)) {
					removeAssociation((Set<UnitServerCuInstance>) viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(USCUAssociationListView.EVENT_LOAD_INSTANCE)) {
					loadUnloadInstance((Set<UnitServerCuInstance>) viewEvent.getEventData(), true);
				} else if (viewEvent.getEventKind().equals(USCUAssociationListView.EVENT_UNLOAD_INSTANCE)) {
					loadUnloadInstance((Set<UnitServerCuInstance>) viewEvent.getEventData(), false);
				} else if (viewEvent.getEventKind().equals(USCUAssociationListView.EVENT_SAVE_ATTRIBUTE_CONFIG)) {
					saveAssociationAttributeConfig((UnitServerCuInstance) viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(USCUAssociationListView.EVENT_LOAD_INSTANCE_ATTRIBUTE)) {
					loadAssociationAttributeConfig((UnitServerCuInstance) viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_EDIT_ALIAS)) {
					openViewByKeyAndClass("EDIT_US_ALIAS", USEditInfoView.class, OpenViewIn.DIALOG, "Edit Unit Server Property");
					notifyEventoToViewWithData(USEditInfoView.EVENT_SET_UNIT_SERVER, this, musp.getUnitServerByIdentification(viewEvent.getEventData().toString()));
				} else if (viewEvent.getEventKind().equals(USEditInfoView.EVENT_CANCELL)) {
					deleteViewByKey("EDIT_US_ALIAS");
				} else if (viewEvent.getEventKind().equals(USEditInfoView.EVENT_SAVE)) {
					musp.updaUnitServerProperty((UnitServer) viewEvent.getEventData());
					updateUnitServerList();
					deleteViewByKey("EDIT_US_ALIAS");
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UPDATE_UNIT_SERVER_LIST)) {
					updateUnitServerList();
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_CU_TYPE_ADD)) {
					openViewByKeyAndClass("US_CU_TYPE_ADD", USAddCuTypeView.class, OpenViewIn.DIALOG, "Add unit type");
					notifyEventoToViewWithData(USAddCuTypeView.EVENT_SET_UNIT_SERVER, this, viewEvent.getEventData().toString());
				} else if (viewEvent.getEventKind().equals(USAddCuTypeView.EVENT_CANCELL)) {
					deleteViewByKey("US_CU_TYPE_ADD");
				} else if (viewEvent.getEventKind().equals(USAddCuTypeView.EVENT_SAVE)) {
					String[] data = (String[]) viewEvent.getEventData();
					musp.addCuTypeToUnitServerAlias(data[0], data[1]);
					unitServerHasBeenSelected(data[0]);
					deleteViewByKey("US_CU_TYPE_ADD");
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_CU_TYPE_REMOVE)) {
					if (unitServerSelected != null) {
						musp.removeCuTypeToUnitServer(unitServerSelected, viewEvent.getEventData().toString());
						unitServerHasBeenSelected(unitServerSelected);
					} else {
						MDSAppView view = getViewByKey("VISTA");
						RefVaadinErrorDialog.shorError(view.getWindow(), "Remove Unit Server Type", "A unit server need to be selected");
					}
				} else if (viewEvent.getEventKind().equals(MDSAppView.EVENT_UNIT_SERVER_CU_TYPE_UPDATE_LIST)) {
					if (unitServerSelected != null) {
						unitServerHasBeenSelected(viewEvent.getEventData().toString());
					} else {
						MDSAppView view = getViewByKey("VISTA");
						RefVaadinErrorDialog.shorError(view.getWindow(), "Remove Unit Server Type", "A unit server need to be selected");
					}
				}

			}
		} catch (Throwable e) {
			MDSAppView view = getViewByKey("VISTA");
			RefVaadinErrorDialog.shorError(view.getWindow(), "Event Error", e.getMessage());
		}
	}

	/**
	 * 
	 * @param eventData
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws RefException
	 */
	private void loadAssociationAttributeConfig(UnitServerCuInstance eventData) throws InstantiationException, IllegalAccessException, ClassNotFoundException, RefException {
		notifyEventoToViewWithData(USCUAssociationListView.EVENT_LOAD_INSTANCE_ATTRIBUTE, this, musp.loadAllAssociationAttributeConfigForUnitServerAlias(eventData));
	}

	/**
	 * 
	 * @param eventData
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws SQLException
	 * @throws RefException
	 */
	private void saveAssociationAttributeConfig(UnitServerCuInstance eventData) throws InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException, RefException {
		musp.saveAllAttributeConfigForAssociation(eventData);
	}

	/**
	 * 
	 * @param loadUnload
	 * @throws Throwable
	 */
	private void loadUnloadAllUnitServerAssociation(boolean loadUnload) throws Throwable {
		Vector<UnitServerCuInstance> instanceForUnitServer = musp.loadAllAssociationForUnitServerAlias(unitServerSelected);
		Set<UnitServerCuInstance> instanceSet = new HashSet<UnitServerCuInstance>();
		instanceSet.addAll(instanceForUnitServer);
		loadUnloadInstance(instanceSet, loadUnload);
	}

	/**
	 * 
	 * @param eventData
	 * @param b
	 * @throws Throwable
	 */
	private void loadUnloadInstance(Set<UnitServerCuInstance> eventData, boolean loadUnload) throws Throwable {
		LoadUnloadWorkUnitSetting setting = new LoadUnloadWorkUnitSetting();
		setting.unit_server_container = musp.getUnitServerByIdentification(unitServerSelected);
		setting.associations = eventData;
		setting.loadUnload = loadUnload;
		SingletonServices.getInstance().getSlowExecution().submitJob(LoadUnloadWorkUnit.class.getName(), setting);
	}

	/**
	 * 
	 * @param associationToRemove
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws SQLException
	 * @throws RefException
	 */
	private void removeAssociation(Set<UnitServerCuInstance> associationToRemove) throws InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException, RefException {
		if (associationToRemove == null || associationToRemove.size() == 0)
			return;
		musp.removeUSCUAssociation(associationToRemove);
		notifyEventoToViewWithData(USCUAssociationListView.EVENT_UPDATE_LIST, this, musp.loadAllAssociationForUnitServerAlias(unitServerSelected));
	}

	/**
	 * 
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws ClassNotFoundException
	 * @throws SQLException
	 * @throws RefException
	 */
	private void showAllAssociationForUnitServer() throws InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException, RefException {
		if (unitServerSelected != null) {
			openViewByKeyAndClass("US_CU_ASSOCIATION_LIST", USCUAssociationListView.class, OpenViewIn.DIALOG, "Work Unit List");
			notifyEventoToViewWithData(USCUAssociationListView.SET_US_ALIAS, this, unitServerSelected);
			notifyEventoToViewWithData(USCUAssociationListView.EVENT_UPDATE_LIST, this, musp.loadAllAssociationForUnitServerAlias(unitServerSelected));
		} else {
			MDSAppView view = getViewByKey("VISTA");
			RefVaadinErrorDialog.shorError(view.getWindow(), "Show all error", "Uniserver not selected");
		}

	}

	private void deleteSelectedUnitServer() throws Throwable {
		if (unitServerSelected != null) {
			musp.deleteUnitServer(unitServerSelected);
			updateUnitServerList();
		}
	}

	private void createNewUnitServer() throws Throwable {
		musp.createNewUnitServer();
		updateUnitServerList();
	}

	/**
	 * 
	 * @throws Throwable
	 */
	private void updateUnitServerList() throws Throwable {
		unitServerHasBeenSelected(null);
		MDSAppView view = getViewByKey("VISTA");
		// load all dataset
		List<UnitServer> allUnitServer = musp.getAllUnitServer();
		Table t = (Table) view.getComponentByKey(MDSAppView.KEY_USERVER_TAB);
		t.removeAllItems();
		for (UnitServer us : allUnitServer) {
			Item woItem = t.addItem(us.getAlias());
			woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_NAME).setValue(us.getAlias());
			woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_ADDRESS).setValue(us.getIp_port());
			woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_HB_TS).setValue(us.getUnitServerHB().getDate());
		}
	}

	/**
	 * A device has been selected so we need to update the datasets
	 * 
	 * @param deviceID
	 * @throws Throwable
	 */
	private void unitServerHasBeenSelected(Object unitServerIdentification) throws Throwable {
		MDSAppView view = getViewByKey("VISTA");
		Table cuTypeTable = (Table) view.getComponentByKey(MDSAppView.KEY_USERVER_CUTYPE_TAB);

		cuTypeTable.removeAllItems();
		if (unitServerIdentification == null)
			return;

		UnitServer us = musp.getUnitServerByIdentification(unitServerIdentification.toString());
		if (us != null) {
			for (String controlUnitType : us.getPublischedCU()) {
				Item woItem = cuTypeTable.addItem(controlUnitType);
				woItem.getItemProperty(MDSAppView.TAB_UNIT_SERVER_CUTYPE_TPE_NAME).setValue(controlUnitType);
			}
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
		MDSAppView view = getViewByKey("VISTA");
		Table dsTable = (Table) view.getComponentByKey(MDSAppView.KEY_DEVICE_TAB);
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
		MDSAppView view = getViewByKey("VISTA");
		Table t = (Table) view.getComponentByKey(MDSAppView.KEY_DEVICE_TAB);
		Object o = t.getValue();
		mdp.deleteDeviceByInstance(o.toString());
		updateDeviceList();
	}

	/**
	 * @throws RefException
	 */
	private void updateAttributeUserModification() throws RefException {
		MDSAppView view = getViewByKey("VISTA");
		try {
			Table t = (Table) view.getComponentByKey(MDSAppView.KEY_DATASET_ATTRIBUTE_TAB);
			@SuppressWarnings("unchecked")
			Collection<DatasetAttribute> datasetAttributes = (Collection<DatasetAttribute>) t.getItemIds();
			for (DatasetAttribute datasetAttributeItemID : datasetAttributes) {
				Object tmpValue = null;
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(MDSAppView.TAB3_TAGS_PATH);
				if (tmpValue != null) {
					datasetAttributeItemID.setTagsPath(tmpValue.toString());
				}
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(MDSAppView.TAB3_RANG_MIN);
				if (tmpValue != null) {
					datasetAttributeItemID.setRangeMin(tmpValue.toString());
				}
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(MDSAppView.TAB3_RANG_MAX);
				if (tmpValue != null) {
					datasetAttributeItemID.setRangeMax(tmpValue.toString());
				}
				tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(MDSAppView.TAB3_DEF_VALUE);
				if (tmpValue != null) {
					datasetAttributeItemID.setDefaultValue(tmpValue.toString());
				}
			}
			mdp.updateDeviceAttributes(datasetAttributes);
		} catch (Throwable e) {
			RefVaadinErrorDialog.shorError(view.getWindow(), "Update Attribute Error", e.getMessage());
			notifyEventoToViewWithData(MDSAppView.EVENT_ATTRIBUTE_EDITING, null, null);
		}
	}

	/**
	 * @throws Throwable
	 */
	private void updateDatasetAttributeList(Dataset dsAttr) throws Throwable {
		MDSAppView view = getViewByKey("VISTA");
		Table dsAttrTable = (Table) view.getComponentByKey(MDSAppView.KEY_DATASET_ATTRIBUTE_TAB);
		dsAttrTable.removeAllItems();
		if (dsAttr == null)
			return;
		List<DatasetAttribute> datasetsAttributeForDataset = mdp.getAttributeForDataset(dsAttr);
		for (DatasetAttribute dsAt : datasetsAttributeForDataset) {
			Item woItem = dsAttrTable.addItem(dsAt);
			woItem.getItemProperty(MDSAppView.TAB3_NAME).setValue(dsAt.getName());
			woItem.getItemProperty(MDSAppView.TAB3_DIR).setValue(dsAt.getNamedDirection());
			woItem.getItemProperty(MDSAppView.TAB3_TYPE).setValue(dsAt.getNamedType());
			woItem.getItemProperty(MDSAppView.TAB3_RANG_MIN).setValue(dsAt.getRangeMin());
			woItem.getItemProperty(MDSAppView.TAB3_RANG_MAX).setValue(dsAt.getRangeMax());
			woItem.getItemProperty(MDSAppView.TAB3_DEF_VALUE).setValue(dsAt.getDefaultValue());
			woItem.getItemProperty(MDSAppView.TAB3_TAGS_PATH).setValue(dsAt.getTagsPath());
		}
	}

	/**
	 * A device has been selected so we need to update the datasets
	 * 
	 * @param deviceID
	 * @throws Throwable
	 */
	private void deviceHasBeenSelected(Object deviceIdentification) throws Throwable {
		MDSAppView view = getViewByKey("VISTA");
		Table dsTable = (Table) view.getComponentByKey(MDSAppView.KEY_DATASET_TAB);
		// Button dsButtonInitiAtStartup = (Button) view.getComponentByKey(DemoAppView.KEY_DEVICE_START_AT_INIT_BUTTON);

		dsTable.removeAllItems();
		updateDatasetAttributeList(null);
		if (deviceIdentification == null)
			return;

		// dsButtonInitiAtStartup.setEnabled(!mdp.isDeviceInitializableAtStartup(deviceIdentification.toString()));

		List<Dataset> datasetsForDevice = mdp.getAllDatasetForDeviceIdentification(deviceIdentification.toString());

		for (Dataset ds : datasetsForDevice) {
			Item woItem = dsTable.addItem(ds);
			woItem.getItemProperty(MDSAppView.TAB2_TIMESTAMP).setValue(ds.getTimestamp().getDate());
			woItem.getItemProperty(MDSAppView.TAB2_ATTR_NUMBER).setValue(ds.getAttributes() != null ? ds.getAttributes().size() : 0);
		}
	}

	/**
	 * @throws Throwable
	 */
	private void updateDeviceList() throws Throwable {
		deviceHasBeenSelected(null);
		MDSAppView view = getViewByKey("VISTA");
		// load all dataset
		List<Device> allDevices = mdp.getAlDevices();
		Table t = (Table) view.getComponentByKey(MDSAppView.KEY_DEVICE_TAB);
		t.removeAllItems();
		for (Device device : allDevices) {
			Item woItem = t.addItem(device.getDeviceIdentification());
			woItem.getItemProperty(MDSAppView.TAB1_DEVICE_INSTANCE).setValue(device.getDeviceIdentification());
			woItem.getItemProperty(MDSAppView.TAB1_CU_PARENT).setValue(device.getCuInstance());
			woItem.getItemProperty(MDSAppView.TAB1_NET_ADDRESS).setValue(device.getNetAddress());
			woItem.getItemProperty(MDSAppView.TAB1_LAST_HB).setValue(device.getLastHeartBeatTimestamp() != null ? device.getLastHeartBeatTimestamp().getDate() : null);
			woItem.getItemProperty("INIT").setValue(device.getInitAtStartup());
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.auth.RefAuthneticatorListener#loggedOut(java.lang.Object[])
	 */
	public void loggedOut(Object... infoData) {
	}

	/*
	 * (non-Javadoc)
	 * 
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
	 * 
	 * @see com.vaadin.terminal.ParameterHandler#handleParameters(java.util.Map)
	 */
	public void handleParameters(Map<String, String[]> parameters) {
		// TODO Auto-generated method stub

	}
}
