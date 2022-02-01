package com.example.first_project;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class ResultService {

    @Autowired
    private ResultRepository repository;

    public Result addResult(Result e) {
        return repository.save(e);
    }

    public Optional<Result> getResult(Long id) {
        return repository.findById(id);
    }

    public List<Result> getAllResults() {
        List<Result> output = new ArrayList<Result>();
        repository.findAll().forEach(output::add);
        return output;
    }

    public Result updateResult(Result e) {
        return repository.save(e);
    }

    public void deleteResult(Result e) {
        repository.delete(e);
}

    public void deleteResult(Long id) {
        repository.deleteById(id);
    }
    public List<Result> getResultByStudent(String student){
        return repository.findByStudent(student);
    }
}