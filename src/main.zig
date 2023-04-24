const std = @import("std");
const hlo = @import("debughello.zig");

pub fn main() !void {
    const stdout_file = std.io.getStdOut().writer();
    var bw = std.io.bufferedWriter(stdout_file);
    const stdout = bw.writer();

    var socket = std.net.StreamServer.init(.{});
    defer socket.deinit();

    try socket.listen(try std.net.Address.resolveIp("127.0.0.1", 9864));

    var bytes = [_]u8{'\x00'} ** 128;
    var read_length: usize = 0;

    std.debug.print("Server Started\n", .{});

    // The action
    try while (socket.accept()) |conn| {
        //exit even if early continue/break
        defer conn.stream.close();
        //get remote connection sysinfo
        std.debug.print("{} says hi\n", .{conn.address});

        read_length = try conn.stream.read(&bytes);
        if (std.mem.eql(u8, bytes[0..read_length], "Hello!")) {
            try hlo.returnHello(&conn);
            continue;
        }

        if (std.mem.eql(u8, bytes[0..read_length], "0")) {
            std.debug.print("got read 0\n", .{});
            try conn.stream.writeAll(&[_]u8{'\x00'});
            _ = try hlo.initSession(&conn, 0);
        }

        std.debug.print("{} says goodbye\n", .{conn.address});
    } else |err| {
        return err;
    };

    try stdout.print("Completed\n", .{});
    try bw.flush();
}
