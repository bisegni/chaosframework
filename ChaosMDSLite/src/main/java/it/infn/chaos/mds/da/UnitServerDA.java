/**
 * 
 */
package it.infn.chaos.mds.da;

import it.infn.chaos.mds.business.DatasetAttribute;
import it.infn.chaos.mds.business.UnitServer;
import it.infn.chaos.mds.business.UnitServerCuInstance;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Vector;

import org.bson.BasicBSONObject;
import org.ref.common.exception.RefException;
import org.ref.common.helper.ExceptionHelper;
import org.ref.server.data.DataAccess;
import org.ref.server.db.sqlbuilder.DeleteSqlBuilder;
import org.ref.server.db.sqlbuilder.InsertUpdateBuilder;
import org.ref.server.db.sqlbuilder.SqlBuilder;

import com.mongodb.util.JSON;

/**
 * @author bisegni
 * 
 */
public class UnitServerDA extends DataAccess {

	/**
	 * 
	 * @return
	 * @throws Throwable
	 */
	public Vector<UnitServer> getAlUnitServer() throws Throwable {
		PreparedStatement ps = null;
		ResultSet rs = null;
		Vector<UnitServer> result = new Vector<UnitServer>();
		SqlBuilder s = new SqlBuilder();
		s.addTable(UnitServer.class);
		s.addPseudoColumntoSelect("*");
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());
			Vector<?> result_partial = executeQueryPreparedStatementAndClose(ps, UnitServer.class, null, null, false);
			for (Object object : result_partial) {
				UnitServer us = (UnitServer) object;
				fillUnitServerWithCUTypes(us);
				result.addElement(us);
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "UnitServerDA::getAlUnitServer");
		} catch (IllegalAccessException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::getAlUnitServer");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 3, "UnitServerDA::getAlUnitServer");
		} finally {
		}
		return result;
	}

	/**
	 * 
	 * @param unitServerAlias
	 * @return
	 * @throws RefException
	 */
	public UnitServer getUnitServerByAlias(String unitServerAlias) throws RefException {
		PreparedStatement ps = null;
		UnitServer result = null;
		SqlBuilder s = new SqlBuilder();
		s.addTable(UnitServer.class);
		s.addPseudoColumntoSelect("*");
		s.addCondition(true, String.format("unit_server_alias='%s'", unitServerAlias));
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());
			result = (UnitServer) executeQuerySingleResultPreparedStatementAndClose(ps, UnitServer.class, null, null, false);
			if (result != null) {
				fillUnitServerWithCUTypes(result);
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "UnitServerDA::getAlUnitServer");
		} catch (IllegalAccessException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::getAlUnitServer");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 3, "UnitServerDA::getAlUnitServer");
		} catch (Throwable e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 3, "UnitServerDA::getAlUnitServer");
		} finally {
		}
		return result;
	}

	public void fillUnitServerWithCUTypes(UnitServer us) throws RefException {
		PreparedStatement ps = null;
		ResultSet rs = null;
		SqlBuilder cuTypesSql = new SqlBuilder();
		cuTypesSql.addTable("unit_server_published_cu");
		cuTypesSql.addPseudoColumntoSelect("control_unit_alias");
		cuTypesSql.addCondition(true, String.format("unit_server_alias='%s'", us.getAlias()));

		try {
			ps = getPreparedStatementForSQLCommand(cuTypesSql.toString());
			rs = ps.executeQuery();
			us.getPublischedCU().clear();
			while (rs.next()) {
				us.getPublischedCU().addElement(rs.getString(1));
			}
		} catch (Exception e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::getAlUnitServer");
		} finally {
			closeResultSet(rs);
			closePreparedStatement(ps);
		}
	}

	/**
	 * Insert new unit server
	 * 
	 * @param deviceID
	 * @throws RefException
	 * @throws SQLException
	 * @throws IllegalAccessException
	 * @throws IllegalArgumentException
	 */
	public void insertNewUnitServer(UnitServer unitServer) throws RefException {
		PreparedStatement ps = null;
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_INSERT);
		iuBuilder.addTable(UnitServer.class);
		iuBuilder.addColumnAndValue(UnitServer.UNIT_SERVER_HB_TIME, "$CURRENT_TIMESTAMP");
		try {
			iuBuilder.fillWithBusinessClass(unitServer);
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			iuBuilder.fillPreparedStatement(ps);
			executeInsertUpdateAndClose(ps);
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::insertNewUnitServer");
		} catch (IllegalAccessException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "UnitServerDA::insertNewUnitServer");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::insertNewUnitServer");
		} finally {
			closePreparedStatement(ps);
		}
	}

	/**
	 * 
	 * @param usci
	 * @throws RefException
	 */
	public void insertNewUSCUAssociation(UnitServerCuInstance usci) throws RefException {
		PreparedStatement ps = null;
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_INSERT);
		iuBuilder.addTable(UnitServerCuInstance.class);
		try {
			iuBuilder.fillWithBusinessClass(usci);
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			iuBuilder.fillPreparedStatement(ps);
			executeInsertUpdateAndClose(ps);
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::insertNewUnitServer");
		} catch (IllegalAccessException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 1, "UnitServerDA::insertNewUnitServer");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::insertNewUnitServer");
		} finally {
			closePreparedStatement(ps);
		}
	}

	/**
	 * Insert new unit server
	 * 
	 * @param deviceID
	 * @throws RefException
	 * @throws SQLException
	 * @throws IllegalAccessException
	 * @throws IllegalArgumentException
	 */
	public void updateUnitServer(UnitServer unitServer) throws RefException {
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuBuilder.addTable(UnitServer.class);
		iuBuilder.addColumnAndValue(UnitServer.UNIT_SERVER_HB_TIME, "$CURRENT_TIMESTAMP");
		iuBuilder.addCondition(true, "unit_server_alias=?");
		PreparedStatement ps = null;
		try {
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			int idx = iuBuilder.fillPreparedStatement(ps);
			ps.setString(idx++, unitServer.getAlias());
			executeInsertUpdateAndClose(ps);
			deletePublishedCU(unitServer);
			insertPublishedCU(unitServer);
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::updateNewUnitServer");
		} finally {
			closePreparedStatement(ps);
		}

	}

	public boolean unitServerAlreadyRegistered(UnitServer unitServer) throws RefException {
		boolean result = false;
		ResultSet rs = null;
		PreparedStatement ps = null;
		SqlBuilder sb = new SqlBuilder();
		sb.addTable(UnitServer.class);
		sb.addPseudoColumntoSelect("count(*)");
		sb.addCondition(true, String.format("%s = ?", UnitServer.UNIT_SERVER_ALIAS));
		try {
			ps = getPreparedStatementForSQLCommand(sb.toString());
			ps.setString(1, unitServer.getAlias());
			rs = ps.executeQuery();
			if (rs.next()) {
				result = rs.getInt(1) > 0;
			}
		} catch (Exception e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::unitServerAlreadyRegistered");
		} finally {
			closeResultSet(rs);
			closePreparedStatement(ps);
		}

		return result;
	}

	public void insertPublishedCU(UnitServer unitServer) throws RefException {
		PreparedStatement ps = null;
		InsertUpdateBuilder iuBuilder = null;
		try {
			for (String cuAlias : unitServer.getPublischedCU()) {
				iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_INSERT);
				iuBuilder.addTable("unit_server_published_cu");
				iuBuilder.addColumnAndValue("unit_server_alias", unitServer.getAlias());
				iuBuilder.addColumnAndValue("control_unit_alias", cuAlias);
				ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
				iuBuilder.fillPreparedStatement(ps);
				executeInsertUpdateAndClose(ps);
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::insertPublishedCU");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::insertPublishedCU");
		} finally {
			closePreparedStatement(ps);
		}

	}

	public void deletePublishedCU(UnitServer unitServer) throws RefException {
		PreparedStatement ps = null;
		DeleteSqlBuilder d = new DeleteSqlBuilder();
		d.addTable("unit_server_published_cu");
		d.addCondition(true, "unit_server_alias = ?");
		try {
			ps = getPreparedStatementForSQLCommand(d.toString());
			ps.setString(1, unitServer.getAlias());
			executeInsertUpdateAndClose(ps);
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::deletePublishedCU");
		} finally {
			closePreparedStatement(ps);
		}
	}

	public Vector<UnitServerCuInstance> returnAllUnitServerCUAssociationbyUSAlias(String unitServerAlias) throws RefException {
		SqlBuilder s = new SqlBuilder();
		Vector<UnitServerCuInstance> result = new Vector<UnitServerCuInstance>();
		s.addTable(UnitServerCuInstance.class);
		s.addPseudoColumntoSelect("*");
		s.addCondition(true, String.format("%s=?", UnitServerCuInstance.UNIT_SERVER_ALIAS));
		PreparedStatement ps = null;
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());

			ps.setString(1, unitServerAlias);
			result = (Vector<UnitServerCuInstance>) executeQueryPreparedStatementAndClose(ps, UnitServerCuInstance.class, null, null, false);
			for (UnitServerCuInstance unitServerCuInstance : result) {
				BasicBSONObject bdriverDescription = (BasicBSONObject) JSON.parse(unitServerCuInstance.getDrvSpec());
				System.out.println(bdriverDescription);
			}
		} catch (Throwable e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::returnAllUnitServerCUAssociationbyUSAlias");
		}

		return result;
	}

	/**
	 * 
	 * @param associationToRemove
	 * @throws RefException
	 * @throws SQLException
	 */
	public void removeAssociation(UnitServerCuInstance associationToRemove) throws RefException {
		DeleteSqlBuilder dsb = new DeleteSqlBuilder();
		dsb.addTable(UnitServerCuInstance.class);
		dsb.addCondition(true, String.format("%s = ?", UnitServerCuInstance.UNIT_SERVER_ALIAS));
		dsb.addCondition(true, String.format("%s = ?", UnitServerCuInstance.CU_ID));
		PreparedStatement ps;
		try {
			ps = getPreparedStatementForSQLCommand(dsb.toString());
			ps.setString(1, associationToRemove.getUnitServerAlias());
			ps.setString(2, associationToRemove.getCuId());
			executeInsertUpdateAndClose(ps);
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::removeAssociation");
		}

	}

	/**
	 * 
	 * @param controlUnitInstance
	 * @return
	 * @throws RefException
	 */
	public boolean cuIDIsMDSManaged(String controlUnitInstance) throws RefException {
		SqlBuilder s = new SqlBuilder();
		s.addTable(UnitServerCuInstance.class);
		s.addPseudoColumntoSelect("count(*)");
		s.addCondition(true, String.format("%s = ?", UnitServerCuInstance.CU_ID));
		PreparedStatement ps;
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());
			ps.setString(1, controlUnitInstance);
			return executeCountFromPreparedStatementAndClose(ps, 1) == 1;
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::cuIDSelfManageable");
		} catch (Exception e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::cuIDSelfManageable");
		}
	}

	public void setState(String deviceIdentification, String state) throws RefException {
		PreparedStatement ps = null;
		InsertUpdateBuilder iuBuilder = new InsertUpdateBuilder(InsertUpdateBuilder.MODE_UPDATE);
		iuBuilder.addTable(UnitServerCuInstance.class);
		iuBuilder.addColumnAndValue(UnitServerCuInstance.STATE, state);
		iuBuilder.addCondition(true, String.format("%s=?", UnitServerCuInstance.CU_ID));
		try {
			ps = getPreparedStatementForSQLCommand(iuBuilder.toString());
			int idx = iuBuilder.fillPreparedStatement(ps);
			ps.setString(idx++, deviceIdentification);
			executeInsertUpdateAndClose(ps);
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::setState");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::setState");
		} finally {
			closePreparedStatement(ps);
		}
	}

	public Vector<DatasetAttribute> loadAllAttributeConfigForAssociation(UnitServerCuInstance associationInstance) throws RefException {
		Vector<DatasetAttribute> result = new Vector<DatasetAttribute>();
		PreparedStatement ps = null;
		ResultSet rs = null;
		SqlBuilder s = new SqlBuilder();
		s.addPseudoColumntoSelect("*");
		s.addTable("attribute_config");
		s.addCondition(true, String.format("%s=?", "unique_id"));
		try {
			ps = getPreparedStatementForSQLCommand(s.toString());
			ps.setString(1, associationInstance.getCuId());
			rs = ps.executeQuery();
			while(rs.next()) {
				DatasetAttribute da = new DatasetAttribute();
				da.setName(rs.getString("attribute_name"));
				da.setDefaultValueNoCheck(rs.getString("default_value"));
				da.setRangeMaxNoCheck(rs.getString("max_value"));
				da.setRangeMinNoCheck(rs.getString("min_value"));
				result.add(da);
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::loadAllAttributeConfigForAssociation");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::loadAllAttributeConfigForAssociation");
		} finally {
			closePreparedStatement(ps);
		}
		
		return result;
	}

	public void saveAllAttributeConfigForAssociation(UnitServerCuInstance associationInstance) throws RefException {
		PreparedStatement ps = null;
		try {
			// first delete all config for device id
			DeleteSqlBuilder d = new DeleteSqlBuilder();
			d.addTable("attribute_config");
			d.addCondition(true, String.format("%s=?", "unique_id"));
			ps = getPreparedStatementForSQLCommand(d.toString());
			ps.setString(1, associationInstance.getCuId());
			executeInsertUpdateAndClose(ps);

			// write all configuration
			InsertUpdateBuilder i = new InsertUpdateBuilder();
			i.addTable("attribute_config");
			i.addColumnAndValue("unique_id", associationInstance.getCuId());

			Vector<DatasetAttribute> conf = associationInstance.getAttributeConfigutaion();
			for (DatasetAttribute datasetAttribute : conf) {
				i.addColumnAndValue("attribute_name", datasetAttribute.getName());
				i.addColumnAndValue("default_value", datasetAttribute.getDefaultValue());
				i.addColumnAndValue("max_value", datasetAttribute.getRangeMax());
				i.addColumnAndValue("min_value", datasetAttribute.getRangeMin());

				ps = getPreparedStatementForInputUpdateBuilder(i);

				executeInsertUpdateAndClose(ps);
			}
		} catch (IllegalArgumentException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 0, "UnitServerDA::setState");
		} catch (SQLException e) {
			throw new RefException(ExceptionHelper.getInstance().putExcetpionStackToString(e), 2, "UnitServerDA::setState");
		} finally {
			closePreparedStatement(ps);
		}
	}

}
