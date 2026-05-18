options gen2

module dn_signal shared public
require dn_uuid public

class DnScopedConnection {
    _handle : DnScopedConnectionHandle

    def release() : void {}
    def is_valid() : bool {}
}

class DnSignal {
    _handle : DnSignalHandle

    def DnSignal() : void {

    }

    def connnect() : DnUUID {}
    def connect_scoped() : ScopedConnection {} // Returns shared_ptr scoped_connection
    def disconnect() : void {}
    def disconnect_all() : void {}
    def emit(/* __VA_ARGS__ */) : bool {}
}

