class Request:
    def __init__(self, time, type, weight, s_point, e_point, id):
        self.start_node = s_point
        self.time = time
        self.type = type
        self.weight = weight
        self.end_node = e_point
        self.id = id
