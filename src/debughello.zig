const std = @import("std");
const ssn = @import("session.zig");

pub fn returnHello(conn: *const std.net.StreamServer.Connection) !void {
    try conn.stream.writeAll("Hello!");
}

pub fn initSession(conn: *const std.net.StreamServer.Connection, offset: u8) !?ssn.Session {
    var bytes = [_]u8{'\x00'} ** 128;

    var username = [_]u8{'\x00'} ** 128;
    var u_length: usize = 0;
    var password = [_]u8{'\x00'} ** 128;
    var p_length: usize = 0;

    try conn.stream.writeAll("login");
    u_length = try conn.stream.read(&bytes);
    std.debug.print("{}, {s}\n", .{ u_length, bytes });
    for (0..u_length) |i| username[i] = bytes[i];
    try conn.stream.writeAll("next");
    p_length = try conn.stream.read(&bytes);
    std.debug.print("{}, {s}\n", .{ p_length, bytes });
    for (0..p_length) |i| password[i] = bytes[i];

    _ = offset;

    std.debug.print("{s}, {s}\n", .{ username, password });

    if (std.mem.eql(u8, "admin", username[0..u_length]) and std.mem.eql(u8, "password", password[0..p_length])) {
        std.debug.print("accept connection\n", .{});
        try conn.stream.writeAll("A4112233");
    } else {
        std.debug.print("deny connection\n", .{});
        try conn.stream.writeAll("Guilty");
    }
    return null;
}
