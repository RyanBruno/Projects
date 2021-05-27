#ifdef TESTS_INTERNAL
char* test_node_data = "127.0.0.1 100600\n127.0.0.1 100601\n127.0.0.1 100602\n";

int test_node_config_parser()
{
    int fd;
    struct peer_node peers[10];
    int n;

    if ((fd = open("temp_test_node_list_file.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0)
        return -1;

    write(fd, test_node_data, strlen(test_node_data)); // TODO write_all
    close(fd);

    if ((n = parse_node_list("temp_test_node_list_file.txt", peers, 10)) < 0)
        return -1;

    remove("temp_test_node_list_file.txt");
    if (n != 3) return -1;
    if (strncmp(test_node_data, peers[0].peer_host, 9)) return -1;
    if (peers[1].peer_procnum != 100601) return -1;
    return 0;
}
#endif
