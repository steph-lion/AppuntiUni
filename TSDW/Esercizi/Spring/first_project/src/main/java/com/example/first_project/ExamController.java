package com.example.first_project;

import java.net.URISyntaxException;
import java.util.List;
import java.util.Optional;

import javax.validation.Valid;

import org.springframework.beans.factory.annotation.Autowired;
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
@RequestMapping("/exams")
class resourceNameController {

    @Autowired
    ExamService examService;

    @GetMapping("/all") //Ritorno tutti gli esami nel db
    public ResponseEntity<List<Exam>> getAllExams(){
        List<Exam> exams=examService.getAllExams();
        return ResponseEntity.ok().body(exams);
    }
    @PostMapping("/add") //Aggiungi un esame
    public ResponseEntity<Exam> addExam(@Valid @RequestBody Exam e) throws URISyntaxException{
        if (e.getId()!=null){
            return ResponseEntity.badRequest().build();
        }
        Exam exam=examService.addExam(e);
        return ResponseEntity.ok().body(exam);
    }
    @GetMapping("{id}") //Cerca un esame by ID
    public ResponseEntity<Exam> getExam(@PathVariable Long id){
        Optional<Exam> exam=examService.getExam(id);
        if (exam.isPresent()) return ResponseEntity.ok().body(exam.get());
        return ResponseEntity.notFound().build();
    }
    @PutMapping("/modify") //Modifica un esame passando la sua struttura
    public ResponseEntity<Exam> updateExam(@Valid @RequestBody Exam e){
        if (e.getId()==null) return ResponseEntity.badRequest().build();
        Exam newExam=examService.updateExam(e);
        return ResponseEntity.ok().body(newExam);
    }
    @DeleteMapping("/delete/{id}") //Cancellare un esame by ID
    public ResponseEntity<List<Exam>> deleteExam(@PathVariable Long id){
        if (!examService.getExam(id).isPresent()) return ResponseEntity.notFound().build();
        examService.deleteExam(id);
        List<Exam> list=examService.getAllExams();
        return ResponseEntity.ok().body(list);
    }
    @GetMapping("/search/{name}") //Cerco un esame con il suo nome "description"
    public ResponseEntity<List<Exam>> searchByName(@PathVariable String name){
        List<Exam> list=examService.getExamsByName(name);
        if (list.isEmpty()) return ResponseEntity.notFound().build();
        return ResponseEntity.ok().body(list);
    }
}