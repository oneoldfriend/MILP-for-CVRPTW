import os


def new_instances_generator(file_name):
    demands = []
    mode = ''
    capacity = None
    no_vehicles = 0
    T_max = 0
    with open(file_name, 'r') as f:
        for line in f:
            line = line.strip(' \t\n')
            if line == "VEHICLE":
                f.readline()
                line = f.readline()
                line = line.strip(' \t\n')
                line = " ".join(line.split()).split(" ")
                no_vehicles = int(line[0])
                capacity = int(line[1])
            elif line == "CUSTOMER":
                f.readline()
                f.readline()
                mode = "customer"
            elif mode == "customer":
                line = line.strip(' \t\n')
                line = " ".join(line.split()).split(" ")
                if line[0] == "0":
                    T_max = int(line[5])
                demands.append([line[1], line[2], line[4],
                                line[5], 1, 1, line[3], line[6]])
    temp_file_name = file_name.split("/")[-1].split(".")[0] + "_new.txt"
    temp_file = open("./instances/Solomon-new/" + temp_file_name, "w")
    temp_file.write("PARAM\n")
    temp_file.write(str(5) + "," + str(1000) + "," +
                    str(2) + "," + str(T_max) + "\n")
    temp_file.write("VEHICLES\n")
    for _ in range(no_vehicles):
        temp_file.write(str(capacity) + "," + str(2) + "\n")
    temp_file.write("DEMANDS\n")
    idx = 0
    for row in demands:
        temp_file.write(
            str(idx) + "," + str(row).replace("[", "").replace("]", "").replace("'", "").replace(" ", "") + "\n")
        idx += 1
    temp_file.close()
    return temp_file_name


all_instances = os.listdir("./instances/Solomon/")
for instance in all_instances:
    new_instances_generator("./instances/Solomon/"+instance)
