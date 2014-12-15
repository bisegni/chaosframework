/**
 * 
 */
package it.infn.chaos.mds.process;

import it.infn.chaos.mds.business.DataServer;
import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.Device;
import it.infn.chaos.mds.business.DeviceClass;
import it.infn.chaos.mds.business.MetaDataServer;
import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.business.UnitServerCuInstance;
import it.infn.chaos.mds.da.DataServerDA;
import it.infn.chaos.mds.da.DeviceDA;
import it.infn.chaos.mds.da.UnitServerDA;

import java.io.File;
import java.io.PrintWriter;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.Vector;

import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;
import org.ref.common.business.BusinessObject;
import org.ref.common.business.DBColumn;
import org.ref.common.business.DBPrimaryKey;
import org.ref.common.business.DBTable;
import org.ref.common.exception.RefException;
import org.ref.server.interapplicationenvironment.ProcessDescription;
import org.ref.server.process.RefProcess;

/**
 * @author Andrea Michelotti
 * 
 */
@ProcessDescription
public class MetaDataServerProcess extends RefProcess{

	
	private static final long serialVersionUID = 1L;
	
	private List<DataServer> dsl;
	private Vector<UnitServer> usv;
	private Vector<DeviceClass> dcv;
	
	
	private void readDB() throws Throwable{
		DataServerDA dsda=(DataServerDA)getDataAccessInstance(DataServerDA.class);
		UnitServerDA usda= (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		
		usv = usda.getAllUnitServer();

		dsl = dsda.getAllDataServer();
		dcv = usda.returnAllClassesBy(null, null, null);
		// load units
		for(UnitServer us:usv){
			// load cu of a unit server
			Vector<UnitServerCuInstance> cuiv =usda.returnAllUnitServerCUAssociationbyUSAlias(us.getAlias());
			for(UnitServerCuInstance cui:cuiv){
				// load attributes
				Vector<DatasetAttribute> ds=usda.loadAllAttributeConfigForAssociation(cui);
				cui.setAttributeConfigutaion(ds);
				
			}
			us.setCuInstances(cuiv);
		}	
	}
	
	public synchronized void dumpConfig(String filename) throws Throwable{
		readDB();
		MetaDataServer mds=new MetaDataServer((Vector<DataServer>) dsl,usv,dcv);
		String out=mds.toBson().toString();
		PrintWriter writer;
		writer = new PrintWriter(filename, "UTF-8");		
		writer.println(out);
		writer.flush();
		writer.close();		
	}
	public void initFromConfig(String filename){
		
	}
	public List<DataServer> getDataServers(){return dsl;}
	public Vector<UnitServer> getUnitServers(){return usv;}
	public Vector<DeviceClass> getClasses(){return dcv;}

	
	/**
	 * @param eventData
	 * @param replace 
	 * @throws Throwable 
	 */
	public synchronized void applyConfig(String filename, Integer replace) throws Throwable {
		// TODO Auto-generated method stub
		
		String text = new Scanner( new File(filename) ).useDelimiter("\\A").next();
		
		MetaDataServer mds=new MetaDataServer(text);
		readDB();
		
		if(replace != 0){
			List<DataServer> dsl_t = new ArrayList<DataServer>();
			Vector<UnitServer> usv_t= new Vector<UnitServer>();
			Vector<DeviceClass> dcv_t = new Vector<DeviceClass>();
			if(replace == 1){
				dsl_t.addAll(dsl);
				usv_t.addAll(usv);
				dcv_t.addAll(dcv);
				
				for(DataServer ds:mds.getDataServers()){
					DataServer found=null;
					for(DataServer ds_current:dsl){
						if(ds.getHostname().equals(ds_current.getHostname())){
							found = ds;
						}
					}
					if(found == null){
						ds.setIdServer(dcv.size()+1);
						dsl_t.add(ds);
					}
				}

				for(UnitServer us:mds.getUSs()){
					UnitServer found=null;
					for(UnitServer us_current:usv){
						if(us.getAlias().equals(us_current.getAlias())){
							found = us;
						}
					}
					if(found == null){
						usv_t.add(us);
					}
				}
				
				for(DeviceClass dc:mds.getDeviceClasses()){
					DeviceClass found=null;
					for(DeviceClass dc_current:dcv){
						if(dc.getDeviceClass().equals(dc_current.getDeviceClass())){
							found = dc;
						}
					}
					if(found == null){
						
						dcv_t.add(dc);
					}
				}
				
			} else if(replace ==2){
				dsl_t.addAll(mds.getDataServers());
				usv_t.addAll(mds.getUSs());
				dcv_t.addAll(mds.getDeviceClasses());
				for(DataServer ds:dsl){
					DataServer found=null;
					for(DataServer ds_current:mds.getDataServers()){
						if(ds.getHostname().equals(ds_current.getHostname())){
							found = ds;
						}
					}
					if(found == null){
						ds.setIdServer(dcv.size()+1);
						dsl_t.add(ds);
					}
				}
				for(UnitServer us:usv){
					UnitServer found=null;
					for(UnitServer us_current:mds.getUSs()){
						if(us.getAlias().equals(us_current.getAlias())){
							found = us;
						}
					}
					if(found == null){
						usv_t.add(us);
					}
				}
				
				for(DeviceClass dc:dcv){
					DeviceClass found=null;
					for(DeviceClass dc_current:mds.getDeviceClasses()){
						if(dc.getDeviceClass().equals(dc_current.getDeviceClass())){
							found = dc;
						}
					}
					if(found == null){
						dcv_t.add(dc);
					}
				}
				
			} else if(replace ==3){
				dsl_t.addAll(dsl);
				usv_t.addAll(usv);
				dcv_t.addAll(dcv);
				for(DataServer ds:mds.getDataServers()){
					DataServer found=null;
					for(DataServer ds_current:dsl){
						if(ds.getHostname().equals(ds_current.getHostname())){
							found = ds_current;
						}
					}
					if(found == null){
						ds.setIdServer(dcv.size()+1);
						dsl_t.add(ds);
					} else {
						found.setHostname(found.getHostname() + "_1");
						found.setIdServer(dcv.size()+1);

						dsl_t.add(found);
					}
				}
				
				for(UnitServer us:mds.getUSs()){
					UnitServer found=null;
					for(UnitServer us_current:usv){
						if(us.getAlias().equals(us_current.getAlias())){
							found = us_current;
						}
					}
					if(found == null){
						usv_t.add(us);
					} else {
						found.setAlias(found.getAlias() + "_1");
						usv_t.add(found);
					}
				}
				
				for(DeviceClass dc:mds.getDeviceClasses()){
					DeviceClass found=null;
					for(DeviceClass dc_current:dcv){
						if(dc.getDeviceClass().equals(dc_current.getDeviceClass())){
							found = dc_current;
						}
					}
					if(found == null){
						dcv_t.add(dc);
					} else {
						found.setDeviceClass(found.getDeviceClass() + "_1");
						dcv_t.add(found);
					}
				}
			}
		usv= usv_t;
		dcv = dcv_t;
		dsl = dsl_t;
		} else {
			dsl = mds.getDataServers();
			usv = mds.getUSs();
			dcv = mds.getDeviceClasses();
		}
		DeviceDA dvda=(DeviceDA)getDataAccessInstance(DeviceDA.class);

		List<Device> dvl =dvda.getAllActiveDevice();
		for(Device d:dvl){
			dvda.deleteDeviceByDeviceIdentification(d.getCuInstance());
		}
		writeDB();
		
		
	}

	/**
	 * @throws Throwable 
	 * 
	 */
	private void writeDB() throws Throwable {
		DataServerDA dsda=(DataServerDA)getDataAccessInstance(DataServerDA.class);
		UnitServerDA usda= (UnitServerDA) getDataAccessInstance(UnitServerDA.class);
		dsda.deleteDataserver(null);
		usda.deleteUnitServer(null);
		usda.deletePublishedCU((String)null);
		usda.removeDeviceClass(null);
		usda.removeAllAttributeConfigurationForAssociation(null);
		commit();
		for(DataServer ds:dsl){
			ds.setIdServer(null);
			dsda.saveServer(ds);
		}
		for(DeviceClass dc:dcv){
			usda.insertNewDeviceClass(dc);
		}
		for(UnitServer us:usv){
			usda.insertNewUnitServer(us);
			for(UnitServerCuInstance cu:us.getCuInstances()){
				
				try {
					usda.addCuTypeToUnitServer(us.getAlias(), cu.getCuType());
				} catch(Throwable e){
					
				}
				usda.insertNewUSCUAssociation(cu);
				usda.saveAllAttributeConfigForAssociation(cu);
				
			}
		}
		
		commit();	

	}	
			
}
