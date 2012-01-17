/**
 * 
 */
package it.infn.chaos.mds.da;

import it.infn.chaos.mds.business.DataServer;

import java.sql.PreparedStatement;
import java.util.List;

import org.ref.server.data.DataAccess;
import org.ref.server.db.sqlbuilder.InsertUpdateBuilder;
import org.ref.server.db.sqlbuilder.SqlBuilder;
import org.ref.server.db.sqlbuilder.SqlTable;

/**
 * @author bisegni
 * 
 */
public class DataServerDA extends DataAccess {
	/**
	 * 
	 * @param newServer
	 * @throws Throwable
	 */
	public void saveServer(DataServer newServer) throws Throwable {
		InsertUpdateBuilder insertUpdateBuilder = new InsertUpdateBuilder();
		insertUpdateBuilder.fillWithBusinessClass(newServer);
		PreparedStatement ps = getPreparedStatementForInputUpdateBuilder(insertUpdateBuilder);
		executeInsertUpdateAndClose(ps);
	}

	/**
	 * 
	 * @param newServer
	 * @throws Throwable
	 */
	public void updateServer(DataServer updatedServer) throws Throwable {
		InsertUpdateBuilder insertUpdateBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		insertUpdateBuilder.fillWithBusinessClass(updatedServer);
		insertUpdateBuilder.addCondition(true, "id_server=?");
		PreparedStatement ps = getPreparedStatementForInputUpdateBuilder(insertUpdateBuilder);
		int idx = insertUpdateBuilder.fillPreparedStatement(ps);
		ps.setInt(idx++, updatedServer.getIdServer());
		executeInsertUpdateAndClose(ps);
	}

	/**
	 * 
	 * @return
	 * @throws Throwable 
	 */
	@SuppressWarnings("unchecked")
	public List<DataServer> getAllDataServer() throws Throwable {
		SqlBuilder select = new SqlBuilder();
		SqlTable t = select.addTable(DataServer.class);
		select.addTableColumnToSelect(t.getTableName(), "*");
		PreparedStatement ps = getPreparedStatementForSQLCommand(select.toString());
		return (List<DataServer>) executeQueryPreparedStatementAndClose(ps, DataServer.class, null, null, false);
	}
}
