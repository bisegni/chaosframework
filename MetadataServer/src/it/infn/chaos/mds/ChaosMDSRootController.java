package it.infn.chaos.mds;

import it.infn.chaos.mds.business.Dataset;
import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.process.ManageDeviceProcess;

import java.util.Collection;
import java.util.List;
import java.util.Observable;

import org.ref.common.exception.RefException;
import org.ref.common.mvc.ViewNotifyEvent;
import org.ref.server.auth.RefAuthneticatorListener;
import org.ref.server.webapp.RefVaadinApplicationController;
import org.ref.server.webapp.RefVaadinMenuManager;
import org.ref.server.webapp.dialog.RefVaadinErrorDialog;

import com.vaadin.data.Item;
import com.vaadin.ui.Component;
import com.vaadin.ui.Table;
import com.vaadin.ui.UriFragmentUtility.FragmentChangedEvent;

@SuppressWarnings("serial")
public class ChaosMDSRootController extends RefVaadinApplicationController implements RefAuthneticatorListener {
	private ManageDeviceProcess	mdp	= null;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.ref.server.webapp.RefVaadinApplicationController#initController()
	 */
	@Override
	public void initController() throws Throwable {
		mdp = (ManageDeviceProcess) openProcess(ManageDeviceProcess.class.getSimpleName(), null);
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
				if (viewEvent.getEventKind().equals(DemoAppView.EVENT_DEVICE_SELECTED)) {
					deviceHasBeenSelected(viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_UPDATE_DEVICE_LIST)) {
					updateDeviceList();
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_DATASET_SELECTED)) {
					updateDatasetAttributeList((Dataset) viewEvent.getEventData());
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_ATTRIBUTE_SAVE)) {
					updateAttributeUserModification();
				} else if (viewEvent.getEventKind().equals(DemoAppView.EVENT_DELETE_SELECTED_DEVICE)) {
					deleteSelectedDevice();
				}else if(viewEvent.getEventKind().equals(DemoAppView.EVENT_SHOW_PREFERENCE)) {
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
				Object tmpValue = t.getItem(datasetAttributeItemID).getItemProperty(DemoAppView.TAB3_DEF_VALUE);
				if (tmpValue != null) {
					datasetAttributeItemID.setDefaultValue(tmpValue.toString());
				}
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
		dsTable.removeAllItems();
		updateDatasetAttributeList(null);
		if (deviceIdentification == null)
			return;

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
			woItem.getItemProperty(DemoAppView.TAB1_LAST_HB).setValue(device.getLastHeartBeatTimestamp()!=null?device.getLastHeartBeatTimestamp().getDate():null);
		}

	}

	@Override
	public void loggedOut(Object... infoData) {
	}

	@Override
	public void loggedIn(Object... infoData) {
	}

	@Override
	public void fragmentChanged(FragmentChangedEvent source) {

	}

	protected void manageFragmentParameters(String[] parts) {
	}

	public void setMenuManager(RefVaadinMenuManager rootWindow) {
	}
}
