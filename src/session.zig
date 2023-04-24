const AuthLevel = enum(u4) {
    read = 0b1000,
    order = 0b1001,
    write = 0b1010,
};

pub const Session = struct {
    session_id: u32,
    //elevated permission is hex A?
    time_started: u64,
    expiration: u64,
    customer_id: u64,
};
