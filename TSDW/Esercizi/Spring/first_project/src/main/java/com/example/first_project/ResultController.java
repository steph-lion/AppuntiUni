package com.example.first_project;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import javax.validation.Valid;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/results")
class ResultController {

    @Autowired
    ResultRepository repository;
    @Autowired
    ResultService resultService;
    @Autowired
    ExamService examService;

    @GetMapping("/all")
    public ResponseEntity<List<Result>> getAllResults(){
        return ResponseEntity.ok().body(resultService.getAllResults());
    }
    @PostMapping("/add")
    public ResponseEntity<Result> addResult(@Valid @RequestBody Result result){
        if (result.getId()!=null) return ResponseEntity.badRequest().build();
        if (!examService.getExam(result.getExam().getId()).isPresent()) return ResponseEntity.notFound().build();
        Result res=resultService.addResult(result);
        return ResponseEntity.ok().body(res);
    }
}