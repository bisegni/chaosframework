package it.infn.chaos.mds.business;

import java.util.List;
import java.util.Vector;

import it.infn.chaos.mds.process.MetaDataServerProcess;

import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;

import com.mongodb.util.JSON;

/**
 * @author Andrea Michelotti
 * 
 */
public class MetaDataServer extends BSONBusinessObject {

	private Vector<DataServer> dsv;
	private Vector<UnitServer> usv;
	private Vector<DeviceClass> dcv;
	private Object bson =null ;
	
	public void fillFromBson(Object json) throws Throwable {
		BasicBSONList blist;
		BasicBSONObject bo =(BasicBSONObject)json;
		dsv=new Vector<DataServer>();
		blist = (BasicBSONList) bo.get("data_servers");
		for (Object object : blist) {
			DataServer ds = new DataServer();
			ds.fillFromBson(object);
			dsv.add(ds);
		}
		
		dcv=new Vector<DeviceClass>();
		blist = (BasicBSONList) bo.get("device_classes");

		for (Object object : blist) {
			DeviceClass ds = new DeviceClass();
			ds.fillFromBson(object);
			dcv.add(ds);
		}
		
		usv=new Vector<UnitServer>();
		blist = (BasicBSONList) bo.get("us");
		for (Object object : blist) {
			UnitServer ds = new UnitServer();
			ds.fillFromBson(object);
			usv.add(ds);
		}
		
	}
	public MetaDataServer(Vector<DataServer> _dsl,Vector<UnitServer> _usv, Vector<DeviceClass> _dcv){
		dsv=_dsl;
		usv=_usv;
		dcv=_dcv;
	}
	/**
	 * @param json
	 * @throws Throwable 
	 */
	public MetaDataServer(String json) throws Throwable {
		bson = JSON.parse(json);
		fillFromBson(bson);		
	}
	
	public Object toBson() throws Throwable {
		BasicBSONObject ret = new BasicBSONObject();
		BasicBSONList blist = new BasicBSONList();
		
		for(DataServer ds:dsv){
			blist.add(ds.toBson());
		}
		ret.append("data_servers", blist);
		BasicBSONList blist2 = new BasicBSONList();

		
		BasicBSONList blist3 = new BasicBSONList();
		for(DeviceClass dc:dcv){
			blist3.add(dc.toBson());
		}
		ret.append("device_classes", blist3);
		for(UnitServer us:usv){
			blist2.add(us.toBson());
		}
		ret.append("us", blist2);
		return ret;
	}
	/**
	 * @return
	 */
	public List<DataServer> getDataServers() {
		// TODO Auto-generated method stub
		return dsv;
	}
	/**
	 * @return
	 */
	public Vector<UnitServer> getUSs() {
		// TODO Auto-generated method stub
		return usv;
	}
	/**
	 * @return
	 */
	public Vector<DeviceClass> getDeviceClasses() {
		// TODO Auto-generated method stub
		return dcv;
	}
	

}
