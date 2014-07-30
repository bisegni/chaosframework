package it.infn.chaos.mds.business;

public class DriverSpec {
	private String	driverName		= "Name";
	private String	driverVersion	= "Version";
	private String	driverInit		= "Initialization parameter";

	public String getDriverName() {
		return driverName;
	}

	public void setDriverName(String driverName) {
		this.driverName = driverName;
	}

	public String getDriverVersion() {
		return driverVersion;
	}

	public void setDriverVersion(String driverVersion) {
		this.driverVersion = driverVersion;
	}

	public String getDriverInit() {
		return driverInit;
	}

	public void setDriverInit(String driverInit) {
		this.driverInit = driverInit;
	}
}