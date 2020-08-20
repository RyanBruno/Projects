
### The RPC protocol

service Greeter {
    rpc Merge (MergeRequest) returns (HelloReply) {}
}

message MergeRequest {
    repeted message set {
        int64 node_id = 1;

        enum node_state {
            ALIVE = 0;
            DELETED = 1;
        }
        node_state tombstone = 2;
        [unknown type] data = 3;

    } = 1;
    repeted map<int64, bytes> merge_set = 1;

}

message MergeReply {
    int32 rc = 1;
}

