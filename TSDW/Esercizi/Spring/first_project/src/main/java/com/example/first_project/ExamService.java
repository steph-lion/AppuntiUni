package com.example.first_project;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class ExamService {

    @Autowired
    private ExamRepository repository;

    public Exam addExam(Exam e) {
        return repository.save(e);
    }

    public Optional<Exam> getExam(Long id) {
        return repository.findById(id);
    }

    public List<Exam> getAllExams() {
        List<Exam> output = new ArrayList<Exam>();
        repository.findAll().forEach(output::add);
        return output;
    }

    public Exam updateExam(Exam e) {
        return repository.save(e);
    }

    public void deleteExam(Exam e) {
        repository.delete(e);
}

    public void deleteExam(Long id) {
        repository.deleteById(id);
    }
    public List<Exam> getExamsByName(String name){
        return repository.findByName(name);
        
    }
}