// AUTO-GENERATED FILE DO NOT EDIT
// See src/mongo/base/generate_error_codes.py
/*    Copyright 2012 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <chaos/common/bson/base/error_codes.h>

#include <boost/static_assert.hpp>

#include <chaos/common/bson/util/mongoutils/str.h>

namespace bson {

    std::string ErrorCodes::errorString(Error err) {
        switch (err) {
        case OK: return "OK";
        case InternalError: return "InternalError";
        case BadValue: return "BadValue";
        case OBSOLETE_DuplicateKey: return "OBSOLETE_DuplicateKey";
        case NoSuchKey: return "NoSuchKey";
        case GraphContainsCycle: return "GraphContainsCycle";
        case HostUnreachable: return "HostUnreachable";
        case HostNotFound: return "HostNotFound";
        case UnknownError: return "UnknownError";
        case FailedToParse: return "FailedToParse";
        case CannotMutateObject: return "CannotMutateObject";
        case UserNotFound: return "UserNotFound";
        case UnsupportedFormat: return "UnsupportedFormat";
        case Unauthorized: return "Unauthorized";
        case TypeMismatch: return "TypeMismatch";
        case Overflow: return "Overflow";
        case InvalidLength: return "InvalidLength";
        case ProtocolError: return "ProtocolError";
        case AuthenticationFailed: return "AuthenticationFailed";
        case CannotReuseObject: return "CannotReuseObject";
        case IllegalOperation: return "IllegalOperation";
        case EmptyArrayOperation: return "EmptyArrayOperation";
        case InvalidBSON: return "InvalidBSON";
        case AlreadyInitialized: return "AlreadyInitialized";
        case LockTimeout: return "LockTimeout";
        case RemoteValidationError: return "RemoteValidationError";
        case NamespaceNotFound: return "NamespaceNotFound";
        case IndexNotFound: return "IndexNotFound";
        case PathNotViable: return "PathNotViable";
        case NonExistentPath: return "NonExistentPath";
        case InvalidPath: return "InvalidPath";
        case RoleNotFound: return "RoleNotFound";
        case RolesNotRelated: return "RolesNotRelated";
        case PrivilegeNotFound: return "PrivilegeNotFound";
        case CannotBackfillArray: return "CannotBackfillArray";
        case UserModificationFailed: return "UserModificationFailed";
        case RemoteChangeDetected: return "RemoteChangeDetected";
        case FileRenameFailed: return "FileRenameFailed";
        case FileNotOpen: return "FileNotOpen";
        case FileStreamFailed: return "FileStreamFailed";
        case ConflictingUpdateOperators: return "ConflictingUpdateOperators";
        case FileAlreadyOpen: return "FileAlreadyOpen";
        case LogWriteFailed: return "LogWriteFailed";
        case CursorNotFound: return "CursorNotFound";
        case UserDataInconsistent: return "UserDataInconsistent";
        case LockBusy: return "LockBusy";
        case NoMatchingDocument: return "NoMatchingDocument";
        case NamespaceExists: return "NamespaceExists";
        case InvalidRoleModification: return "InvalidRoleModification";
        case ExceededTimeLimit: return "ExceededTimeLimit";
        case ManualInterventionRequired: return "ManualInterventionRequired";
        case DollarPrefixedFieldName: return "DollarPrefixedFieldName";
        case InvalidIdField: return "InvalidIdField";
        case NotSingleValueField: return "NotSingleValueField";
        case InvalidDBRef: return "InvalidDBRef";
        case EmptyFieldName: return "EmptyFieldName";
        case DottedFieldName: return "DottedFieldName";
        case RoleModificationFailed: return "RoleModificationFailed";
        case CommandNotFound: return "CommandNotFound";
        case DatabaseNotFound: return "DatabaseNotFound";
        case ShardKeyNotFound: return "ShardKeyNotFound";
        case OplogOperationUnsupported: return "OplogOperationUnsupported";
        case StaleShardVersion: return "StaleShardVersion";
        case WriteConcernFailed: return "WriteConcernFailed";
        case MultipleErrorsOccurred: return "MultipleErrorsOccurred";
        case ImmutableField: return "ImmutableField";
        case CannotCreateIndex: return "CannotCreateIndex";
        case IndexAlreadyExists: return "IndexAlreadyExists";
        case AuthSchemaIncompatible: return "AuthSchemaIncompatible";
        case ShardNotFound: return "ShardNotFound";
        case ReplicaSetNotFound: return "ReplicaSetNotFound";
        case InvalidOptions: return "InvalidOptions";
        case InvalidNamespace: return "InvalidNamespace";
        case NodeNotFound: return "NodeNotFound";
        case WriteConcernLegacyOK: return "WriteConcernLegacyOK";
        case NoReplicationEnabled: return "NoReplicationEnabled";
        case OperationIncomplete: return "OperationIncomplete";
        case CommandResultSchemaViolation: return "CommandResultSchemaViolation";
        case UnknownReplWriteConcern: return "UnknownReplWriteConcern";
        case RoleDataInconsistent: return "RoleDataInconsistent";
        case NoClientContext: return "NoClientContext";
        case NoProgressMade: return "NoProgressMade";
        case RemoteResultsUnavailable: return "RemoteResultsUnavailable";
        case UniqueIndexViolation: return "UniqueIndexViolation";
        case IndexOptionsConflict: return "IndexOptionsConflict";
        case IndexKeySpecsConflict: return "IndexKeySpecsConflict";
        case CannotSplit: return "CannotSplit";
        case SplitFailed: return "SplitFailed";
        case NotMaster: return "NotMaster";
        case DuplicateKey: return "DuplicateKey";
        case InterruptedAtShutdown: return "InterruptedAtShutdown";
        case Interrupted: return "Interrupted";
        case BackgroundOperationInProgressForDatabase: return "BackgroundOperationInProgressForDatabase";
        case BackgroundOperationInProgressForNamespace: return "BackgroundOperationInProgressForNamespace";
        case OutOfDiskSpace: return "OutOfDiskSpace";
        case KeyTooLong: return "KeyTooLong";
        default: return bsonutils::str::stream() << "Location" << err;
        }
    }

    ErrorCodes::Error ErrorCodes::fromString(const StringData& name) {
        if (name == "OK") return OK;
        if (name == "InternalError") return InternalError;
        if (name == "BadValue") return BadValue;
        if (name == "OBSOLETE_DuplicateKey") return OBSOLETE_DuplicateKey;
        if (name == "NoSuchKey") return NoSuchKey;
        if (name == "GraphContainsCycle") return GraphContainsCycle;
        if (name == "HostUnreachable") return HostUnreachable;
        if (name == "HostNotFound") return HostNotFound;
        if (name == "UnknownError") return UnknownError;
        if (name == "FailedToParse") return FailedToParse;
        if (name == "CannotMutateObject") return CannotMutateObject;
        if (name == "UserNotFound") return UserNotFound;
        if (name == "UnsupportedFormat") return UnsupportedFormat;
        if (name == "Unauthorized") return Unauthorized;
        if (name == "TypeMismatch") return TypeMismatch;
        if (name == "Overflow") return Overflow;
        if (name == "InvalidLength") return InvalidLength;
        if (name == "ProtocolError") return ProtocolError;
        if (name == "AuthenticationFailed") return AuthenticationFailed;
        if (name == "CannotReuseObject") return CannotReuseObject;
        if (name == "IllegalOperation") return IllegalOperation;
        if (name == "EmptyArrayOperation") return EmptyArrayOperation;
        if (name == "InvalidBSON") return InvalidBSON;
        if (name == "AlreadyInitialized") return AlreadyInitialized;
        if (name == "LockTimeout") return LockTimeout;
        if (name == "RemoteValidationError") return RemoteValidationError;
        if (name == "NamespaceNotFound") return NamespaceNotFound;
        if (name == "IndexNotFound") return IndexNotFound;
        if (name == "PathNotViable") return PathNotViable;
        if (name == "NonExistentPath") return NonExistentPath;
        if (name == "InvalidPath") return InvalidPath;
        if (name == "RoleNotFound") return RoleNotFound;
        if (name == "RolesNotRelated") return RolesNotRelated;
        if (name == "PrivilegeNotFound") return PrivilegeNotFound;
        if (name == "CannotBackfillArray") return CannotBackfillArray;
        if (name == "UserModificationFailed") return UserModificationFailed;
        if (name == "RemoteChangeDetected") return RemoteChangeDetected;
        if (name == "FileRenameFailed") return FileRenameFailed;
        if (name == "FileNotOpen") return FileNotOpen;
        if (name == "FileStreamFailed") return FileStreamFailed;
        if (name == "ConflictingUpdateOperators") return ConflictingUpdateOperators;
        if (name == "FileAlreadyOpen") return FileAlreadyOpen;
        if (name == "LogWriteFailed") return LogWriteFailed;
        if (name == "CursorNotFound") return CursorNotFound;
        if (name == "UserDataInconsistent") return UserDataInconsistent;
        if (name == "LockBusy") return LockBusy;
        if (name == "NoMatchingDocument") return NoMatchingDocument;
        if (name == "NamespaceExists") return NamespaceExists;
        if (name == "InvalidRoleModification") return InvalidRoleModification;
        if (name == "ExceededTimeLimit") return ExceededTimeLimit;
        if (name == "ManualInterventionRequired") return ManualInterventionRequired;
        if (name == "DollarPrefixedFieldName") return DollarPrefixedFieldName;
        if (name == "InvalidIdField") return InvalidIdField;
        if (name == "NotSingleValueField") return NotSingleValueField;
        if (name == "InvalidDBRef") return InvalidDBRef;
        if (name == "EmptyFieldName") return EmptyFieldName;
        if (name == "DottedFieldName") return DottedFieldName;
        if (name == "RoleModificationFailed") return RoleModificationFailed;
        if (name == "CommandNotFound") return CommandNotFound;
        if (name == "DatabaseNotFound") return DatabaseNotFound;
        if (name == "ShardKeyNotFound") return ShardKeyNotFound;
        if (name == "OplogOperationUnsupported") return OplogOperationUnsupported;
        if (name == "StaleShardVersion") return StaleShardVersion;
        if (name == "WriteConcernFailed") return WriteConcernFailed;
        if (name == "MultipleErrorsOccurred") return MultipleErrorsOccurred;
        if (name == "ImmutableField") return ImmutableField;
        if (name == "CannotCreateIndex") return CannotCreateIndex;
        if (name == "IndexAlreadyExists") return IndexAlreadyExists;
        if (name == "AuthSchemaIncompatible") return AuthSchemaIncompatible;
        if (name == "ShardNotFound") return ShardNotFound;
        if (name == "ReplicaSetNotFound") return ReplicaSetNotFound;
        if (name == "InvalidOptions") return InvalidOptions;
        if (name == "InvalidNamespace") return InvalidNamespace;
        if (name == "NodeNotFound") return NodeNotFound;
        if (name == "WriteConcernLegacyOK") return WriteConcernLegacyOK;
        if (name == "NoReplicationEnabled") return NoReplicationEnabled;
        if (name == "OperationIncomplete") return OperationIncomplete;
        if (name == "CommandResultSchemaViolation") return CommandResultSchemaViolation;
        if (name == "UnknownReplWriteConcern") return UnknownReplWriteConcern;
        if (name == "RoleDataInconsistent") return RoleDataInconsistent;
        if (name == "NoClientContext") return NoClientContext;
        if (name == "NoProgressMade") return NoProgressMade;
        if (name == "RemoteResultsUnavailable") return RemoteResultsUnavailable;
        if (name == "UniqueIndexViolation") return UniqueIndexViolation;
        if (name == "IndexOptionsConflict") return IndexOptionsConflict;
        if (name == "IndexKeySpecsConflict") return IndexKeySpecsConflict;
        if (name == "CannotSplit") return CannotSplit;
        if (name == "SplitFailed") return SplitFailed;
        if (name == "NotMaster") return NotMaster;
        if (name == "DuplicateKey") return DuplicateKey;
        if (name == "InterruptedAtShutdown") return InterruptedAtShutdown;
        if (name == "Interrupted") return Interrupted;
        if (name == "BackgroundOperationInProgressForDatabase") return BackgroundOperationInProgressForDatabase;
        if (name == "BackgroundOperationInProgressForNamespace") return BackgroundOperationInProgressForNamespace;
        if (name == "OutOfDiskSpace") return OutOfDiskSpace;
        if (name == "KeyTooLong") return KeyTooLong;
        return UnknownError;
    }

    ErrorCodes::Error ErrorCodes::fromInt(int code) {
        return static_cast<Error>(code);
    }

    bool ErrorCodes::isNetworkError(Error err) {
        switch (err) {
        case HostUnreachable:
        case HostNotFound:
            return true;
        default:
            return false;
        }
    }

    bool ErrorCodes::isInterruption(Error err) {
        switch (err) {
        case Interrupted:
        case InterruptedAtShutdown:
        case ExceededTimeLimit:
            return true;
        default:
            return false;
        }
    }

    bool ErrorCodes::isIndexCreationError(Error err) {
        switch (err) {
        case CannotCreateIndex:
        case IndexOptionsConflict:
        case IndexKeySpecsConflict:
        case IndexAlreadyExists:
            return true;
        default:
            return false;
        }
    }


namespace {
    BOOST_STATIC_ASSERT(sizeof(ErrorCodes::Error) == sizeof(int));
}  // namespace
}  // namespace bson
