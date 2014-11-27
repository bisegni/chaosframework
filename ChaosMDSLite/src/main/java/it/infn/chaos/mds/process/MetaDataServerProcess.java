/**
 * 
 */
package it.infn.chaos.mds.process;

import it.infn.chaos.mds.business.DataServer;
import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.DeviceClass;
import it.infn.chaos.mds.business.MetaDataServer;
import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.business.UnitServerCuInstance;
import it.infn.chaos.mds.da.DataServerDA;
import it.infn.chaos.mds.da.UnitServerDA;

import java.io.File;
import java.io.PrintWriter;
import java.sql.SQLException;
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
		dcv = usda.returnAllClassesBy(null, null);
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
	public void applyConfig(String filename, Integer replace) throws Throwable {
		// TODO Auto-generated method stub
		
		String text = new Scanner( new File(filename) ).useDelimiter("\\A").next();
		
		MetaDataServer mds=new MetaDataServer(text);
		
		if(replace == 0){
			// just replace
			dsl = mds.getDataServers();
			usv = mds.getUSs();
			dcv = mds.getDeviceClasses();
		} else {
			
			for(DataServer ds_current:dsl){
				for(DataServer ds:mds.getDataServers()){
					if(ds.getHostname() == ds_current.getHostname()){
						if(replace == 2){
							// keep config
							dsl.remove(ds_current);
							dsl.add(ds);
						} else if(replace==3){
							String ren=ds.getHostname() + "_1";
							ds.setHostname(ren);
							dsl.add(ds);
						}
					}
					
				}
			}
		
			for(UnitServer us_current:usv){
				for(UnitServer us:mds.getUSs()){
					if(us.getAlias() == us_current.getAlias()){
						if(replace == 2){
							// keep config
							usv.remove(us_current);
							usv.add(us);
						} else if(replace==3){
							String ren=us.getAlias() + "_1";
							us.setAlias(ren);
							usv.add(us);
						}
					}
					
				}
			}
			for(DeviceClass dc_current:dcv){
				for(DeviceClass dc:mds.getDeviceClasses()){
					if(dc.getDeviceClass() == dc_current.getDeviceClass()){
						if(replace == 2){
							// keep config
							dcv.remove(dc_current);
							dcv.add(dc);
						} else if(replace==3){
							String ren=dc.getDeviceClass() + "_1";
							dc.setDeviceClass(ren);
							dcv.add(dc);
						}
					}
					
				}
			}
			
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
			dsda.saveServer(ds);
		}
		for(DeviceClass dc:dcv){
			usda.insertNewDeviceClass(dc);
		}
		for(UnitServer us:usv){
			usda.insertNewUnitServer(us);
			for(UnitServerCuInstance cu:us.getCuInstances()){
				usda.addCuTypeToUnitServer(us.getAlias(), cu.getCuType());
				usda.insertNewUSCUAssociation(cu);
				usda.saveAllAttributeConfigForAssociation(cu);
				
			}
		}
		
		commit();	

	}	
			
}
